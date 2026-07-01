#include "Misc.hpp"
#include "../frontend/TypedNumber.hpp"
#include <limits>
#include <string.h>
#include <algorithm>
#define IssueWarning(tokenPtr, errorMsg, ...) logger->IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

Ast::Node *BuildInitializerList(const std::vector<Ast::Node>& initItems, PagedHeap *allocator)
{
    Ast::Node* initializerList = allocator->allocate<Ast::Node>();
    initializerList->type = Ast::initializer_list;

    size_t len = initItems.size();
    initializerList->lChild = allocator->allocateArray<Ast::Node>(len);
    memcpy(initializerList->lChild, initItems.data(), sizeof(Ast::Node) * len);
    initializerList->rChild = lenToAstPtr(len);

    return initializerList;
}

ArraySize GetArrayElemCount(const AccessArray *accArray, Logger *logger, NodeExecutor *ne)
{
    bool hitPointer = false;
    bool hitArray = false;
    uint64_t arrayCount = 1;

    AccessType* accType = accArray->ptr;
    for(size_t i = 0; i < accArray->count; i++)
    {
        if(accType[i].type == ACC_POINTER)
        {
            hitPointer = true;
            break;
        }
        else if(accType[i].type == ACC_ARRAY)
        {
            hitArray = true;
            arrayCount *= accType[i].array.size;
        }
        else if(ACC_ARRAY_VLA)
        {
            if(hitArray)
            {
                IssueWarning(nullptr, "Variable length array can be only in first dimension")
            }

            Typed::Number num = ne->ExecuteNode(accType[i].array.asmExpr);
            if(num.type == Typed::d_float || num.type == Typed::d_double)
            {
                IssueWarning(nullptr, "Floats cannot be used inside array size declaration")
            }
            arrayCount *= num.int64;
        }
    }

    return {arrayCount, hitPointer};
}

MemoryDesc GetMemoryDesc(const AccessArray *accArray, SymbolType *type, Logger *logger, NodeExecutor *ne)
{
    ArraySize arrSize = GetArrayElemCount(accArray, logger, ne);
    if(arrSize.hitPointer)
    {
        return {arrSize.elementCount * 8, 8};
    }
   return {arrSize.elementCount * type->size, type->alignment};
}

const Ast::Node *GetFirstNestedValue(const Ast::Node *designatorList, Logger *logger)
{
    if(designatorList->type != Ast::initializer_list)
    {
        return designatorList;
    }

    while (designatorList->type == Ast::initializer_list)
    {
        if(astPtrToLen(designatorList->rChild) == 0)
        {
            return nullptr;
        }
        if(designatorList->lChild->lChild->type != Ast::initializer_list)
        {
            if(designatorList->lChild->rChild)
            {
                IssueWarning(nullptr, "initialization of of non-aggregate type with designated innitializer list");
            }
            return designatorList->lChild;
        }
        designatorList = designatorList->lChild->lChild;
    }
    
    return nullptr;
}

std::vector<ArrayInitPair> PartitionArrayInitializer(const Ast::Node *designatorList, const AccessArray* nextAcc, Logger* logger, NodeExecutor* nodeExec, PagedHeap* allocator)
{
    // first step resolve position of each element in the array
    std::vector<ArrayInitPair> pairs;
    std::vector<Ast::Node> arrayExprs;
    uint64_t linearSize = 0;
    size_t i = 0;
    while(i < astPtrToLen(designatorList->rChild))
    {
        const Ast::Node* item = &designatorList->lChild[i];
        const Ast::Node* desigExpr  = item->rChild;
        const Ast::Node* subInitializerList = nullptr;
        uint64_t pos;
        // calculate place for initializer 
        if(desigExpr)
        {
            Typed::Number num = nodeExec->ExecuteNode(desigExpr->rChild->lChild);
            linearSize = Typed::CastTo<uint64_t>(num);
        }
        pos = linearSize;
        linearSize++;
        // resolve list of expressions 
        if(item->lChild->type == Ast::initializer_list)
        {
            if(allocator)
            {
                subInitializerList = item->lChild;
            }
            item++;
            i++;
        }
        else
        {
            // iterate per element
            uint64_t elemCount = 0;
            if(nextAcc->count > 0)
            {
                elemCount = GetArrayElemCount(nextAcc, logger, nodeExec).elementCount;
            } 
            else
            {
                // it means array is lowest one so every element is to be treated as single value
                elemCount = 1; 
            }
            while (elemCount > 0 && i < astPtrToLen(designatorList->rChild))
            {
                const Ast::Node* expr = item->lChild->type != Ast::initializer_list ? 
                            item: 
                            GetFirstNestedValue(item->lChild, logger);
                if(allocator && desigExpr)
                {
                    // designator must be removed so allocate new node in place
                    Ast::Node* replacement = allocator->allocate<Ast::Node>();
                    replacement->type = expr->type;
                    replacement->token = expr->token;
                    replacement->lChild = expr->lChild;
                    expr = replacement;
                }
                arrayExprs.push_back(*expr);
                item++;
                i++;
                elemCount--;

                if(item->rChild)
                {
                    // if element has designator it marks we need to process it as another element in the array
                    break;
                }
            }
            if(allocator)
            {
                subInitializerList = BuildInitializerList(arrayExprs, allocator);
            }
        }

        pairs.push_back({pos, subInitializerList});
        arrayExprs.clear();
    }

    std::sort(pairs.begin(), pairs.end(),
            [](const ArrayInitPair& l, const ArrayInitPair& r){
                return l.idx < r.idx;
            });

    return pairs;
}

bool IsPointer(const AccessArray *accArray, size_t startIdx )
{
    for(size_t i = startIdx; i < accArray->count; i++)
    {
        const AccessType* currAcc = &accArray->ptr[i];
        if(currAcc->type == ACC_POINTER)
        {
            return true;
        }
        else if(currAcc->type == ACC_ARRAY)
        {
            // do nothing 
        }
        else if(currAcc->type == ACC_FN_CALL)
        {
            return false;
        }
        else if(currAcc->type == ACC_FN_DECL)
        {
            return false;
        }
    }

    return false;
}

bool DecaysToPointer(const AccessArray *accArray, size_t startIdx)
{
    for(size_t i = startIdx; i < accArray->count; i++)
    {
        const AccessType* currAcc = &accArray->ptr[i];
        if(currAcc->type == ACC_POINTER)
        {
            return true;
        }
        else if(currAcc->type == ACC_ARRAY)
        {
            return true;
        }
        else if(currAcc->type == ACC_FN_CALL)
        {
            return false;
        }
        else if(currAcc->type == ACC_FN_DECL)
        {
            return false;
        }
    }

    return false;
}

bool IsArray(const AccessArray *accArray)
{
    if(accArray->count == 0)
    {
        return false;
    }

    const AccessType* currAcc = &accArray->ptr[0];
    if(currAcc->type == ACC_POINTER)
    {
        return false;
    }
    else if(currAcc->type == ACC_ARRAY)
    {
        return true;
    }
    else if(currAcc->type == ACC_FN_CALL)
    {
        return false;
    }
    else if(currAcc->type == ACC_FN_DECL)
    {
        return false;
    }
 

    return false;
}

std::string_view GetBuiltInName(const BuiltIn::Type type)
{
    switch (type)
    {
    case BuiltIn::int_1:      return "i1";   break;
    case BuiltIn::bool_t:      return "i8";   break;
    case BuiltIn::s_char_8:    return "i8";   break;
    case BuiltIn::u_char_8:    return "i8";   break;
    case BuiltIn::s_int_16:    return "i16";  break;
    case BuiltIn::u_int_16:    return "i16";  break;
    case BuiltIn::s_int_32:    return "i32";  break;
    case BuiltIn::u_int_32:    return "i32";  break;
    case BuiltIn::s_int_64:    return "i64";  break;
    case BuiltIn::u_int_64:    return "i64";  break;
    case BuiltIn::float_32:    return "float";   break;
    case BuiltIn::double_64:   return "double";  break;
    case BuiltIn::long_double: return "x86_fp80";  break;
    case BuiltIn::ptr:         return "ptr";     break;
    case BuiltIn::void_t:      return "void";     break;
    default:
        printf("code gen: type unsupported");
        exit(-1);
        break;
    }
    return "";
}

BuiltIn::Type GetBuiltInType(const std::string_view &typeName)
{
    if(typeName == "i8")       return BuiltIn::s_char_8;   // ambiguous
    if(typeName == "i16")      return BuiltIn::s_int_16;   // ambiguous
    if(typeName == "i32")      return BuiltIn::s_int_32;   // ambiguous
    if(typeName == "i64")      return BuiltIn::s_int_64;   // ambiguous
    if(typeName == "float")    return BuiltIn::float_32;
    if(typeName == "double")   return BuiltIn::double_64;
    if(typeName == "x86_fp80") return BuiltIn::long_double;
    if(typeName == "ptr")      return BuiltIn::ptr;
    if(typeName == "void")     return BuiltIn::void_t;
    return BuiltIn::none;
}

uint32_t GetBuiltInAlignment(const BuiltIn::Type type)
{
    switch (type)
    {
    case BuiltIn::bool_t:      return 1;
    case BuiltIn::s_char_8:    return 1;
    case BuiltIn::u_char_8:    return 1;
    case BuiltIn::s_int_16:    return 2;
    case BuiltIn::u_int_16:    return 2;
    case BuiltIn::s_int_32:    return 4; 
    case BuiltIn::u_int_32:    return 4;
    case BuiltIn::s_int_64:    return 8;
    case BuiltIn::u_int_64:    return 8;
    case BuiltIn::float_32:    return 4;
    case BuiltIn::double_64:   return 8;
    case BuiltIn::long_double: return 16;
    case BuiltIn::ptr:         return 8;
    case BuiltIn::void_t:      return 0;
    default:
        printf("non built in type");
        exit(-1);
        break;
    }
    return 0;
}

std::string_view GetViewForToken(const Token &token, FileManager* fm)
{
    FILE_STATE state;
    if(fm->GetFileState(&token.location.id, &state) != 0)
    {
        printf("Preprocessor critical error: Requested file does not exit\n");
        exit(-1);
    }

    // removes \" from both start and end 
    uint8_t offset = token.type == TokenType::string_literal ? 1 : 0;
    std::string_view tokenView(state.fileData + token.location.offset + offset,
                                token.location.len - offset * 2);
    return tokenView;
}

Ast::NodeType tokenAsmToExpr(TokenType::Type asmToken)
{
    switch (asmToken)
    {
    case TokenType::star_equal: return Ast::op_multiply;
    case TokenType::slash_equal: return Ast::op_divide;
    case TokenType::percent_equal: return Ast::op_divide_modulo;
    case TokenType::plus_equal: return Ast::op_add;
    case TokenType::minus_equal: return Ast::op_minus;
    case TokenType::l_shift_equal: return Ast::op_l_shift;
    case TokenType::r_shift_equal: return Ast::op_r_shift;
    case TokenType::ampresand_equal: return Ast::op_and;
    case TokenType::caret_equal: return Ast::op_exc_or;
    case TokenType::pipe_equal: return Ast::op_inc_or;
    default:return Ast::none;
    }
}
