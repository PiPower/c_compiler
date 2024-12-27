#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <string>
#include <unordered_map>
#include <vector>

enum class SymbolClass
{
    TYPE,
    VARIABLE,
    FUNCTION,
};

struct Symbol
{
    SymbolClass type;
};

struct SymbolTable
{
    std::unordered_map<std::string, Symbol*> symbols;
    SymbolTable* parent;
    // 0 is for global scope
    uint64_t scopeLevel; 
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
    // bit 0 is defined
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
    // bit 0 is defined
    uint64_t attributes;
    // does not include args
    uint64_t fnStackSize;
    SymbolTable localSymtab;
    std::vector<std::string*> argTypes;
    std::vector<std::string*> argNames;
    SymbolFunction()
    :
    type(SymbolClass::VARIABLE), retType(nullptr), attributes(0)
    {}
};

typedef std::unordered_map<std::string, Symbol*>::iterator SymtabIter;

#endif