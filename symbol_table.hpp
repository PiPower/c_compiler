#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

/*
    varType(SymbolVariable) and retType(SymbolFunction) are strings 
    encoded in the following manner:
    name | qualifiers #_opt pt_1_qualifiers, ... #_opt pt_k_qualifiers
    name - is string of name_size bytes representing symbol name 
    qualifiers - byte for qualifiers regarding type
    '#' means that variable is a pointer, number of '#' stands for 
    pointer order 
    pt_j_qualifiers - qualifiers for jth pointer
*/


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
    SymbolClass symClass;
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
    SymbolClass symClass;
    bool isBuiltIn;
    // bit 0: isDefined
    uint64_t attribs;
    uint64_t typeSize;
    // if symbol consists of many subtypes like struct
    // store references types themselves and their names
    // in the following vectors
    std::vector<std::string> types;
    std::vector<std::string> names;

};

struct SymbolVariable
{
    SymbolClass symClass;
    std::string* varType;
    // bit 0 is defined
    uint64_t attributes;

    SymbolVariable()
    :
    symClass(SymbolClass::VARIABLE), varType(nullptr), attributes(0)
    {}
};


struct SymbolFunction
{
    SymbolClass symClass;
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
    symClass(SymbolClass::VARIABLE), retType(nullptr), attributes(0)
    {}
};

typedef std::unordered_map<std::string, Symbol*>::iterator SymtabIter;

#endif