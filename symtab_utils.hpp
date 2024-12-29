#include "symbol_table.hpp"

#ifndef SYMTAB_UTILS
#define SYMTAB_UTILS

#define GET_SYMBOL(parser, name)  getSymbol((parser)->symtab, (name)) 
#define GET_SYMBOL_EX(parser, name, ptr)  getSymbol((parser)->symtab, (name), (ptr) ) 
#define SET_SYMBOL(parser, name, symbol) (parser)->symtab->symbols[(name)] = (symbol)
#define SYMTAB_CEND(parser) (parser)->symtab->symbols.cend()

Symbol* getSymbol(SymbolTable* symtab, const std::string& name, uint64_t* scopeLevel = nullptr);

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