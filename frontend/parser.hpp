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
    void* contextData;
};


std::vector<AstNode*> parse( Scanner* scanner, SymbolTable* symtab);
#endif