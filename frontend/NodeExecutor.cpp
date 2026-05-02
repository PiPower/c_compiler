#include "NodeExecutor.hpp"
#include "../utils/DataEncoder.hpp"
#include "../frontend/SemanticAnalysis.hpp"

#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl("NodeExecutor", tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

template<typename _Tp>
struct right_shift 
{
    _GLIBCXX14_CONSTEXPR
    _Tp
    operator()(const _Tp& __x, const _Tp& __y) const
    { return __x >> __y; }
};

template<typename _Tp>
struct left_shift
{
    _GLIBCXX14_CONSTEXPR
    _Tp
    operator()(const _Tp& __x, const _Tp& __y) const
    { return __x << __y; }
};

template<typename Op>
Typed::Number BinaryOp(NodeExecutor* ne, const Ast::Node* node)
{
    Typed::Number l = ne->ExecuteNode(node->lChild);
    Typed::Number r = ne->ExecuteNode(node->rChild);

    Typed::Number out{};
    out.int64 = Op{}(l.int64, r.int64);
    out.type = Typed::d_int64_t;

    return out;
}

template<typename Op>
Typed::Number UnaryOp(NodeExecutor* ne, const Ast::Node* node)
{
    Typed::Number l = ne->ExecuteNode(node->lChild);

    Typed::Number out{};
    out.int64 = Op{}(l.int64);
    out.type = Typed::d_int64_t;

    return out;
}

NodeExecutor::NodeExecutor(FileManager *fm, SemanticAnalyzer* sm)
:
fm(fm), logger(fm), sema(sm)
{
}

AccessDesc NodeExecutor::parseAccess(const AccessType *accType)
{
    bool hitPointer = false;
    int64_t arrayCount = 1;

    while (accType)
    {
        if(accType->type == ACC_POINTER)
        {
            hitPointer = true;
            break;
        }
        else if(accType->type == ACC_ARRAY)
        {
            if(!accType->array.asmExpr)
            {
                arrayCount = 0;
            }
            else
            {
                Typed::Number num = ExecuteNode(accType->array.asmExpr);
                if(num.type == Typed::d_float || num.type == Typed::d_double)
                {
                    printf("Floats cannot be used inside array size declaration \n");
                    exit(-1);
                }
                arrayCount *= num.int64;
            }
         
        }
        accType = accType->next;
    }
    
    return {hitPointer, (size_t)arrayCount};
}

Typed::Number NodeExecutor::ExecuteNode(const Ast::Node *expr)
{
    Typed::Number numOut = {};
    if(!expr)
    {
        return numOut;
    }

    numOut.type = Typed::d_int64_t;
    switch (expr->type)
    {
    case Ast::NodeType::constant:
        if(expr->token.isFloat) 
        {
           IssueWarning(&expr->token, "Expression must have integral type");
           exit(-1);
        }
        numOut.int64 = stringToInt64(GetDataPtr(&expr->token), 
                expr->token.location.len, GetTokenMode(expr->token));
        return numOut;
    case Ast::NodeType::string_literal:
        IssueWarning(&expr->token, "Expression must have integral type");
        exit(-1);
        break;
    case Ast::NodeType::character:
        if(expr->token.location.len > 3)
        {
            IssueWarning(&expr->token, "Multi byte character constants  are not supported\n");
            exit(-1);
        }
        //numOut.int64 = stringToChar(GetDataPtr(&expr->token), expr->token.location.len);
        return numOut;
    case Ast::NodeType::cond_expression:
    {
        Ast::Node* condition = expr->lChild;
        Ast::Node* exprTrue = expr->lChild + 1;
        Ast::Node* exprFalse = expr->rChild;

        Typed::Number condValue = ExecuteNode(condition);
        if(condValue.int64 != 0) {numOut = ExecuteNode(exprTrue);}
        else{numOut = ExecuteNode(exprFalse);}
        return numOut;
    }
    case Ast::NodeType::op_sizeof:
    {
        const Ast::Node* typeName = expr->lChild;
        const Ast::Node* specQualList = typeName->rChild;
        const Ast::Node* decl = typeName->lChild;
        if(!decl->lChild && !decl->rChild)
        {
            DeclSpecs spec = sema->AnalyzeDeclSpec(specQualList);
            SymbolType* symType = sema->symTab->QueryTypeSymbol(spec.typenameView);
            return {.int64 = (int64_t)symType->size, .type = Typed::DType::d_int64_t };
        }
        else
        {
            Declarator declarator = sema->AnalyzeDeclarator(decl);
            AccessDesc desc = parseAccess(&declarator.accessTypes);
            if(desc.hitPtr) 
            {
                return {.int64 = (int64_t)(POINTER_SIZE * desc.arraySize), .type = Typed::DType::d_int64_t };
            }
            else 
            {
                DeclSpecs spec = sema->AnalyzeDeclSpec(specQualList);
                SymbolType* symType = sema->symTab->QueryTypeSymbol(spec.typenameView);
                return {.int64 = (int64_t)(symType->size * desc.arraySize), .type = Typed::DType::d_int64_t };
            }
        }
    }
    // unary ops
    case Ast::NodeType::op_log_negate: return UnaryOp<std::logical_not<int64_t>>(this, expr);
    case Ast::NodeType::op_minus: return UnaryOp<std::negate<int64_t>>(this, expr);
    case Ast::NodeType::op_complement: return UnaryOp<std::bit_not<int64_t>>(this, expr);
    // binary ops
    case Ast::NodeType::op_less_equal: return BinaryOp<std::less_equal<int64_t>>(this, expr);
    case Ast::NodeType::op_less: return BinaryOp<std::less<int64_t>>(this, expr);
    case Ast::NodeType::op_greater_equal: return BinaryOp<std::greater_equal<int64_t>>(this, expr);
    case Ast::NodeType::op_greater: return BinaryOp<std::greater<int64_t>>(this, expr);
    case Ast::NodeType::op_l_shift: return BinaryOp<left_shift<int64_t>>(this, expr);
    case Ast::NodeType::op_r_shift: return BinaryOp<right_shift<int64_t>>(this, expr);
    case Ast::NodeType::op_inc_or: return BinaryOp<std::bit_or<int64_t>>(this, expr);
    case Ast::NodeType::op_exc_or: return BinaryOp<std::bit_xor<int64_t>>(this, expr);
    case Ast::NodeType::op_and: return BinaryOp<std::bit_and<int64_t>>(this, expr);
    case Ast::NodeType::op_log_and: return BinaryOp<std::logical_and<int64_t>>(this, expr);
    case Ast::NodeType::op_log_or: return BinaryOp<std::logical_or<int64_t>>(this, expr);
    case Ast::NodeType::op_divide: return BinaryOp<std::divides<int64_t>>(this, expr);
    case Ast::NodeType::op_divide_modulo: return BinaryOp<std::modulus<int64_t>>(this, expr);
    case Ast::NodeType::op_subtract: return BinaryOp<std::minus<int64_t>>(this, expr);
    case Ast::NodeType::op_add: return BinaryOp<std::plus<int64_t>>(this, expr);
    case Ast::NodeType::op_multiply: return BinaryOp<std::multiplies<int64_t>>(this, expr);
    case Ast::NodeType::op_equal: return BinaryOp<std::equal_to<int64_t>>(this, expr);
    case Ast::NodeType::op_not_equal: return BinaryOp<std::not_equal_to<int64_t>>(this, expr);
    case Ast::NodeType::expression: return ExecuteNode(expr->lChild);
    // forbiden element
    case Ast::NodeType::op_pre_inc:
    case Ast::NodeType::op_post_inc:
    case Ast::NodeType::op_pre_dec:
    case Ast::NodeType::op_post_dec:
    case Ast::NodeType::assignment:
    case Ast::NodeType::function_call:
    case Ast::NodeType::args_expr_list:
    case Ast::NodeType::cast:
    case Ast::NodeType::get_addr:
    case Ast::NodeType::dref_ptr:
    case Ast::NodeType::array_access:
    case Ast::NodeType::struct_access:
    case Ast::NodeType::ptr_access:
    default:
        IssueWarning(&expr->token,
        "Operation [%s] is not allowed in preprocessing directive \n",
        Ast::nodeStr(expr->type));
        exit(-1);
        break;
    }
    return numOut;
}

const char *NodeExecutor::GetDataPtr(const Token *token)
{
    FILE_STATE state;
    fm->GetFileState(&token->location.id, &state);
    
    return state.fileData + token->location.offset;
}

uint8_t NodeExecutor::GetTokenMode(const Token &token)
{
    if(token.isDec){ return MODE_DEC;}
    else if(token.isHex){ return MODE_HEX;}
    else if(token.isOct){ return MODE_OCT;}
    return MODE_BIN;
}
