#include "../../include/frontend/parser.hpp"

#include <string>
using namespace std;

static AstNode* primaryExpression(Scanner& scanner)
{
    Token token = scanner.popToken();
    AstNode* root;
    switch (token.type)
    {
    case TokenType::IDENTIFIER:
        root = new AstNode{NodeType::IDENTIFIER, {}, NodeDataType::NONE, new string()};
        *((string*)root->context.arbitraryData) = *(string*)token.context.data;
        return root;
    case TokenType::CONSTANT:
        root = new AstNode{NodeType::CONSTANT, {}, NodeDataType::INT_32};
        root->context.int_32 = token.context.int_32;
        return root;
    default:
        fprintf(stdout, "Unexpected token for primary expression \n");
        exit(-1);
        break;
    }
}

AstNode* parseExpression(Scanner& scanner)
{

}