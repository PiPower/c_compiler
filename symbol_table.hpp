#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

/*
    varType(SymbolVariable) and retType(SymbolFunction) are strings 
    encoded in the following manner:
    name | qualifiers #_opt pt_1_qualifiers, ... pt_k_qualifiers
    name - is string of name_size bytes representing symbol name 
    qualifiers - byte for qualifiers regarding type
    '#' means that variable is a pointer, number of '#' stands for 
    pointer order 
    pt_j_qualifiers - qualifiers for jth pointer


Type casting

    Types are divided into four groups.
    Implicit conversions between groups are not allwed,
    except for INT_*_S and INT_*_U. During this cast
    stronger is used, and treated as an offset into INT_*_S
    group. This type of implicit conversion triggers compiler 
    warning.
    Demoting implicit conversion within grups triggers warning.
    In special group implicit conversions between any members are not allwed
    unless stated later
*/


#include <string>
#include <unordered_map>
#include <vector>

enum class SymbolClass
{
    TYPE,
    VARIABLE,
    FUNCTION,
    ALIAS,
};

struct Symbol
{
    SymbolClass symClass;
};

struct SymbolTable
{
    std::vector<Symbol*> symbols;
    std::unordered_map<std::string, size_t> symId;
    SymbolTable* parent;
    // 0 is for global scope
    uint64_t scopeLevel; 
};

/*
attributes:
    bit0: is type defined
    bit1: is type translated to hardware
if symbol consists of many subtypes (like struct)
store references to variable names and names of their types 
int the types and names vectors
Base types can be differentiated by lack of subtypes(ie names.size() == 0)
dataAlignment is alignment of individual members of struct
*/
struct SymbolType
{
    SymbolClass symClass;
    uint16_t affiliation;
    uint64_t typeSize;
    uint64_t dataAlignment;
    uint64_t attributes;
    uint8_t sysVGr;
    // if symbol consists of many subtypes like struct
    // store references types themselves and their names
    // in the following vectors
    std::vector<std::string> types;
    std::vector<std::string> qualifiers;
    std::vector<std::string> names;
    std::vector<uint32_t> paramOffset;

};

struct SymbolAlias
{
    SymbolClass symClass;
    std::string* realName;
};

/*
    attributes:
    bit0 is variable defined
*/
struct SymbolVariable
{
    SymbolClass symClass;
    std::string* varType;
    std::string* qualifiers;
    uint64_t attributes;

    SymbolVariable()
    :
    symClass(SymbolClass::VARIABLE), varType(nullptr), attributes(0)
    {}
};

/*
    attributes:
    bit0 is function defined
*/
struct SymbolFunction
{
    SymbolClass symClass;
    std::string* retType;
    std::string* qualifiers;
    uint64_t attributes;
    // does not include args
    uint64_t fnStackSize;
    SymbolTable localSymtab;
    std::vector<std::string*> argTypes;
    std::vector<std::string*> argNames;
    std::vector<std::string*> argQualifiers;
    SymbolFunction()
    :
    symClass(SymbolClass::VARIABLE), retType(nullptr), attributes(0)
    {}
};

typedef std::unordered_map<std::string, size_t>::iterator IdIter;

#endif