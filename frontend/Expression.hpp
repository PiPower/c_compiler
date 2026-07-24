#pragma once
#include <stdint.h>
#include "../LangTypes.hpp"
#include "TypedNumber.hpp"

constexpr int64_t INDEX_INVALID = -1;
constexpr int64_t EXPR_ID_EMPTY= -10;
constexpr int64_t EXPR_ID_GLOBAL = -100;
constexpr int64_t EXPR_ID_CONST = -1000;
constexpr int64_t EXPR_ID_IGNORE = -10000;
constexpr int64_t EXPR_ID_VAR = -20000; // used to conver that ExprRet holds symbolic value representing raw variable
constexpr int64_t EXPR_ID_FN = -20001;
/*
    sema supports anonymous struct members
*/

struct ExprRet
{
    BuiltIn::Type type;
    Typed::Number num;
    int64_t id;
    uint8_t isPtr : 1; // this indicates whether expr is pure llvm-pointer
    uint32_t internalPtrCount; // pointers defined from c code are defined here
    std::string_view typenameView; 
    const SymbolType* symType;
    union
    {
        const SymbolVariable* var;
        const SymbolFunction* fn;
    };

    ExprRet() 
    : type(BuiltIn::none), num({}), id(0), isPtr(0), internalPtrCount(0), typenameView(""), symType(nullptr), var(nullptr){};
    ExprRet(BuiltIn::Type type, const Typed::Number& num, int64_t id, uint8_t isPtr = 0, uint32_t internalPtrCount = 0) 
    : type(type), num(num), id(id), isPtr(isPtr), internalPtrCount(internalPtrCount), 
      typenameView(""), symType(nullptr), var(nullptr){}

};