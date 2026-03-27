#pragma once
#include <stdint.h>
#include "../SymbolTable.hpp"

// Most of the type name will be accesible via single token stored in tokenizer
// yet more annoying types like "long long unsigned int" have to be concatanated from many tokens
// The more annoying type names will be allocated using viewHandle, then accessed via strView
struct DeclSpecs
{
    TypeBits declType; 
    BuiltIn::Type dataType;
    const char* typenameHandle;
    std::string_view typenameView; 
};

