#ifndef PARSER
#define PARSER

#include <vector>
#include "./scanner.hpp"
#include "./parser_utils.hpp"

/*
Ast is executed left-right 
*/
struct AstNode
{
    NodeType nodeType;
    std::vector<AstNode*> children;
    NodeDataType nodeDataType;
    union
    {
        void* arbitraryData;
        int int_32;
    } context;
    
};


std::vector<AstNode*> parse(Scanner& scanner);
AstNode* parseExpression(Scanner& scanner);
AstNode* parseConstantExpression(Scanner& scanner);

AstNode* parseDeclaration(Scanner& scanner);
AstNode* parseDeclarator(Scanner& scanner);
AstNode* parseDirectDeclarator(Scanner& scanner);

#endif