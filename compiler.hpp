#ifndef COMPILER_H
#define COMPILER_H
#include "symbol_table.hpp"


void compile(const char* file);
void initSymbolTalbe(SymbolTable* symTab);
#endif