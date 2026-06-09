#pragma once
#include <stdint.h>
#include "../LangTypes.hpp"
#include "TypedNumber.hpp"

constexpr int64_t EXPR_ID_GLOBAL = -100;
constexpr int64_t EXPR_ID_CONST = -1000;
constexpr int64_t EXPR_ID_IGNORE = -10000;
constexpr int64_t EXPR_ID_VAR = -20000; // used to conver that ExprRet holds symbolic value representing raw variable
/*
    sema supports anonymous struct members
*/

struct ExprRet
{
    BuiltIn::Type type;
    Typed::Number num;
    int64_t id;
    const SymbolVariable* var;

    ExprRet() = default;
    ExprRet(BuiltIn::Type type, const Typed::Number& num, int64_t id) 
    : type(type), num(num), id(id) {}
};