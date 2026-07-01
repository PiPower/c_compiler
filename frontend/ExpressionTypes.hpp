#pragma once 
#include "SemanticAnalysis.hpp"
//dodać koncept
template<typename OP>
ExprRet BinaryOp(SemanticAnalyzer* sema, CodeGen* cg, const Ast::Node* root)
{
    ExprRet left = {}, right = {}, out = {};
    ExprRet oldLeft = sema->LoadVariable(sema->AnalyzeExpr(root->lChild));
    ExprRet oldRight = sema->LoadVariable(sema->AnalyzeExpr(root->rChild));
    sema->HandleTypePromotion(&oldLeft, &oldRight, &left, &right);

    if constexpr(OP::forcedOutput == BuiltIn::none)
    {
        out.type = left.type;
    }
    else
    {
        out.type = OP::forcedOutput;
    }

    if(left.id == EXPR_ID_CONST && right.id == EXPR_ID_CONST)
    {
        out.id = EXPR_ID_CONST;
        out.num = Typed::TypedBinOp<OP::template op>(left.num, right.num);
    }
    else 
    {
        out.id = OP::emitBinExpr(cg, left.type, {left.id, left.num}, {right.id, right.num});
    }
    return out;
}

struct BinaryAddition
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<typename T>
    using op = std::plus<T>; 

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalAddition(opType, left, right);
    }
};

struct BinarySubtraction
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<typename T>
    using op = std::minus<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalSubtraction(opType, left, right);
    }
};

struct BinaryMultiplication
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<typename T>
    using op = std::multiplies<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalMultiplication(opType, left, right);
    }
};

struct BinaryDivision
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<typename T>
    using op = std::divides<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalDivision(opType, left, right);
    }
};

struct BinaryModulus
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<std::integral T>
    using op = std::modulus<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalModulus(opType, left, right);
    }
};

struct BinaryBitAnd
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<std::integral T>
    using op = std::bit_and<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalBitAnd(opType, left, right);
    }
};

struct BinaryBitOr
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<std::integral T>
    using op = std::bit_or<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalBitOr(opType, left, right);
    }
};

struct BinaryBitXor
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<std::integral T>
    using op = std::bit_xor<T>;

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalBitXor(opType, left, right);
    }
};

struct BinaryShiftLeft
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<std::integral T>
    struct op
    {
        constexpr T operator()(T lhs, T rhs) const
        {
            return lhs << rhs;
        }
    };

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalShiftLeft(opType, left, right);
    }
};

struct BinaryShiftRight
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::none;

    template<std::integral T>
    struct op
    {
        constexpr T operator()(T lhs, T rhs) const
        {
            return lhs >> rhs;
        }
    };

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalShiftRight(opType, left, right);
    }
};

struct CmpGreater
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::int_1;

    template<typename T>
    using op = std::greater<T>; 

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalCmpGe(opType, left, right);
    }
};

struct CmpGreaterEq
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::int_1;

    template<typename T>
    using op = std::greater_equal<T>; 

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalCmpGeEq(opType, left, right);
    }
};

struct CmpLess
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::int_1;

    template<typename T>
    using op = std::less<T>; 

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalCmpLe(opType, left, right);
    }
};

struct CmpLessEq
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::int_1;

    template<typename T>
    using op = std::less_equal<T>; 

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalCmpLeEq(opType, left, right);
    }
};

struct CmpEqual
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::int_1;

    template<typename T>
    using op = std::equal_to<T>; 

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalCmpEq(opType, left, right);
    }
};

struct CmpNotEqual
{
    static constexpr BuiltIn::Type forcedOutput = BuiltIn::int_1;

    template<typename T>
    using op = std::not_equal_to<T>; 

    static int64_t emitBinExpr(CodeGen* cg, BuiltIn::Type opType, Operator left, Operator right)
    {
        return cg->EmitLocalCmpNotEq(opType, left, right);
    }
};