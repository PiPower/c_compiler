#pragma once
#include <stdint.h>
#include "../LangTypes.hpp"
#include "TypedNumber.hpp"

constexpr int64_t EXPR_ID_CONST = -1000;
constexpr int64_t EXPR_ID_IGNORE = -10000;
/*
    sema supports anonymous struct members
*/

struct ExprRet
{
    BuiltIn::Type type;
    Typed::Number num;
    int64_t id;
};