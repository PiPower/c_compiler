#ifndef PARSER
#define PARSER

#include <vector>
#include "./scanner.hpp"
#include "./parser_utils.hpp"

struct AstNode
{
    NodeType nodeType;
    std::vector<AstNode*> children;
    NodeDataType nodeDataType;
};


std::vector<AstNode*> parse(Scanner& scanner);
AstNode* parseDeclaration(Scanner& scanner, Token type);
AstNode* parseDeclarator(Scanner& scanner, Token token);
AstNode* parseDirectDeclarator(Scanner& scanner, Token token);

#endif