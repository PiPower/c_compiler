#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <unordered_map>

struct Symbol
{
    /* data */
};


struct SymbolTable
{
    std::unordered_map<std::string, Symbol> symbols;
};



void compile(const char* file);

#endif