#ifndef COMPILER_H
#define COMPILER_H
#include "symtab_utils.hpp"


void compile(const char* file, FILE* stream);
void initSymbolTalbe(SymbolTable* symTab);
#endif