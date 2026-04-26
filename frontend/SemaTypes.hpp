#pragma once
#include <stdint.h>
#include "../SymbolTable.hpp"

// Most of the type name will be accesible via single token stored in tokenizer
// yet more annoying types like "long long unsigned int" have to be concatanated from many tokens
// The more annoying type names will be allocated using viewHandle, then accessed via strView
struct DeclSpecs
{
    TypeBits declType; 
    std::string_view typenameView; 
};

struct Declarator
{
    AccessType accessTypes;
    std::string_view name;
};

struct InitDeclarator
{
    Declarator decl;
    const Ast::Node* initializer;
};

struct StructDeclarator
{
    Declarator decl;
    int64_t bitCount;
};

struct StructDeclaration
{
    DeclSpecs declSpec;
    std::vector<StructDeclarator> declarators;
};

struct MemoryDesc
{
    uint64_t size;
    uint32_t alignment;
};