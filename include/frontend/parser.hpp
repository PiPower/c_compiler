#ifndef PARSER
#define PARSER

#include <vector>
#include "./scanner.hpp"

enum class NodeType
{

};

struct AstNode
{
    NodeType nodeType;
    std::vector<AstNode*> children;
};


std::vector<AstNode*> parse(Scanner& scanner);

AstNode* parseDeclaration(Scanner& scanner, Token type);
inline bool isTypeSpecifier(Token& token);
#endif