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
        long int int_32;
        long int int_64;
    } context;
    
};


std::vector<AstNode*> parse(Scanner& scanner);
// parser for expressions
AstNode* parseExpression(Scanner& scanner);
AstNode* parseConstantExpression(Scanner& scanner);
AstNode* assignmentExpression(Scanner& scanner);
// parser for declarations
AstNode* parseDeclaration(Scanner& scanner);
// parser for statements
AstNode* parseStatementAndDeclaration(Scanner& scanner);
AstNode* parseStatement(Scanner& scanner);
AstNode* parseSelectionStatement(Scanner& scanner);
AstNode* parseCompoundStatement(Scanner& scanner);
AstNode* parseLabeledStatement(Scanner& scanner);
AstNode* parseIterationStatement(Scanner& scanner);
AstNode* parseJumpStatement(Scanner& scanner);

#endif