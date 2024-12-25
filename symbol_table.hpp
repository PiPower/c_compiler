#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <string>
#include <unordered_map>
#include <vector>

enum class SymbolClass
{
    TYPE,
    VARIABLE,
    FUNCTION
};

struct Symbol
{
    SymbolClass type;
};

struct SymbolType
{
    SymbolClass type;
    bool isBuiltIn;
    uint64_t typeSize;
    // if symbol consists of many subtypes like struct
    // store references types themselves and their names
    // in the following vectors
    std::vector<std::string> types;
    std::vector<std::string> names;

};

struct SymbolVariable
{
    SymbolClass type;
    std::string* varType;
    // bit0 : isGlobal
    uint64_t attributes;

    SymbolVariable()
    :
    type(SymbolClass::VARIABLE), varType(nullptr), attributes(0)
    {}
};

struct SymbolFunction
{
    SymbolClass type;
    std::string* retType;
    uint64_t attributes;

    SymbolFunction()
    :
    type(SymbolClass::VARIABLE), retType(nullptr), attributes(0)
    {}
};

struct SymbolTable
{
    std::unordered_map<std::string, Symbol*> symbols;
};
typedef std::unordered_map<std::string, Symbol*>::iterator SymtabIter;

#define GET_SYMBOL(parser, name) (parser)->symtab->symbols.find( (name) )
#define SET_SYMBOL(parser, name) (parser)->symtab->symbols[(name)]
#define SYMTAB_CEND(parser) (parser)->symtab->symbols.cend()

inline constexpr void setGlobalAttr(SymbolVariable* var)
{
    var->attributes |= 0x01;
}

inline constexpr void disableGlobalAttr(SymbolVariable* var)
{
    var->attributes &=  ~0x01;
}

inline constexpr bool isGlobalAttr(SymbolVariable* var)
{
    return var->attributes & 0x01 > 0 ;
}
#endif