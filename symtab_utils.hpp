#include "symbol_table.hpp"

#ifndef SYMTAB_UTILS
#define SYMTAB_UTILS

#define GET_SYMBOL(parser, name)  getSymbol((parser)->symtab, (name)) 
#define GET_SYMBOL_EX(parser, name, ptr)  getSymbol((parser)->symtab, (name), (ptr) ) 
#define SET_SYMBOL(parser, name, symbol) (parser)->symtab->symbols[(name)] = (symbol)
#define SYMTAB_CEND(parser) (parser)->symtab->symbols.cend()
//-----------------
// type groups
// signed int group
// bits 0, 1
#define INT8_S 0x00
#define INT16_S 0x01
#define INT32_S 0x02
#define INT64_S 0x03
//-----------------
// unsigned int group
// bits 2, 3
#define INT8_U (0x00 << 2)
#define INT16_U (0x01 << 2)
#define INT32_U (0x02 << 2)
#define INT64_U (0x03 << 2)
//-----------------
// float group
// bits 4, 5
#define FLOAT32 (0x00 << 4)
#define DOUBLE64 (0x01 << 4)
#define DOUBLE128 (0x02 << 4)
//#define UNUSED (0x03 << 4)
//-----------------
//special group
// bits 6, 7
#define POINTER_GR (0x00 << 6)
#define VOID_GR (0x01 << 6)
#define STRUCT_GR (0x02 << 6)
//#define UNUSED (0x03 << 6)

Symbol* getSymbol(SymbolTable* symtab, const std::string& name, uint64_t* scopeLevel = nullptr);
Symbol* getSymbolLocal(SymbolTable* symtab, const std::string& name);
// function symbol
inline constexpr void setDefinedAttr(SymbolFunction* fn)
{
    fn->attributes |= 0x01;
}

inline constexpr void disableDefinedAttr(SymbolFunction* fn)
{
    fn->attributes &=  ~0x01;
}

inline constexpr bool isSetDefinedAttr(SymbolFunction* fn)
{
    return fn->attributes & 0x01 > 0 ;
}

// variable symbol
inline constexpr void setDefinedAttr(SymbolVariable* var)
{
    var->attributes |= 0x01;
}

inline constexpr void disableDefinedAttr(SymbolVariable* var)
{
    var->attributes &=  ~0x01;
}

inline constexpr bool isSetDefinedAttr(SymbolVariable* var)
{
    return var->attributes & 0x01 > 0 ;
}

// symbol type
inline constexpr void setDefinedAttr(SymbolType* type)
{
    type->attributes |= 0x01;
}

inline constexpr void disableDefinedAttr(SymbolType* type)
{
    type->attributes &=  ~0x01;
}

inline constexpr bool isSetDefinedAttr(SymbolType* type)
{
    return type->attributes & 0x01 > 0 ;
}

#endif