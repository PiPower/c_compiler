#ifndef CODE_GEN_UTILS
#define CODE_GEN_UTILS
#include "../symbol_table.hpp"

void fillTypeHwdInfo(SymbolType* symType);
// alignment is first power of 2 that is <= type_size
uint32_t getTypeAlignment(SymbolType* symType);
#endif