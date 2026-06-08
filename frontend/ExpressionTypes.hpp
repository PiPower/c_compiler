#pragma once 
#include "SemanticAnalysis.hpp"

template<typename OP>
ExprRet BinaryOp(SemanticAnalyzer* sema, CodeGen* cg, const Ast::Node* root)
{
    ExprRet left = {}, right = {}, out = {};
    sema->BinaryExprProlog(&left, &right, root->lChild, root->rChild);

    out.type = left.type;
    if(left.id == EXPR_ID_CONST && right.id == EXPR_ID_CONST)
    {
        out.id = EXPR_ID_CONST;
        out.num = Typed::TypedBinOp<OP::template  op>(left.num, right.num);
    }
    else 
    {
        out.id = OP::emitBinExpr(cg, left.type, {left.id, left.num}, {right.id, right.num});
    }
    return out;
}

struct BinaryAddition
{
    template<typename T>
    using op = std::plus<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalAddition(opType, left, right);
    }
};

struct BinarySubtraction
{
    template<typename T>
    using op = std::minus<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalSubtraction(opType, left, right);
    }
};

struct BinaryMultiplication
{
    template<typename T>
    using op = std::multiplies<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalMultiplication(opType, left, right);
    }
};

struct BinaryDivision
{
    template<typename T>
    using op = std::divides<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalDivision(opType, left, right);
    }
};
