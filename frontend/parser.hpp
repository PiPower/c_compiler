#ifndef PARSER_H
#define PARSER_H

#include "parser_utils.hpp"
#include "scanner.hpp"
#include "../compiler.hpp"
#include <vector>

struct AstNode
{
    NodeType nodeType;
    std::vector<AstNode*> children;
    // for block data is pointer to symbol table for that block
    // for other nodes it is either not used or used as string
    std::string* data;
    std::string* type;
};

struct NodeAllocator;

std::vector<AstNode*> parse( Scanner* scanner, SymbolTable* symtab, NodeAllocator *allocator);
#endif