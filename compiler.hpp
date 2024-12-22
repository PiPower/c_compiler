#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <unordered_map>
#include <vector>

enum class SymbolClass
{
    TYPE
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

};

struct SymbolTable
{
    std::unordered_map<std::string, Symbol*> symbols;
};



void compile(const char* file);
void initSymolTalbe(SymbolTable* symTab);
#endif