#include "../../include/frontend/parser.hpp"

#include <string>
using namespace std;

typedef AstNode* (* parseFunctionPtr)(Scanner& scanner);
static AstNode* parseLoop(Scanner& scanner, parseFunctionPtr parsingFunction, AstNode* root, vector<TokenType> types)
{
    while (scanner.currentTokenOneOf(types))
    {
        Token operatorToken = scanner.popToken();
        AstNode* parent = new AstNode{tokenMathTypeToNodeType(operatorToken), {}, NodeDataType::INFERED};
        AstNode* right = parsingFunction(scanner);
        parent->children.push_back(root);
        parent->children.push_back(right);

        root = parent;
    }

    return root;
}


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


static AstNode* postfixExpression(Scanner& scanner)
{
    return primaryExpression(scanner);
}


static AstNode* unaryExpression(Scanner& scanner)
{
    return postfixExpression(scanner);
}

static AstNode* castExpression(Scanner& scanner)
{
    return unaryExpression(scanner);
}

static AstNode* multiplicativeExpression(Scanner& scanner)
{
    AstNode* root = castExpression(scanner);
    return parseLoop(scanner, castExpression, root, {TokenType::STAR, TokenType::SLASH, TokenType::PERCENT});
}

static AstNode* additiveExpression(Scanner& scanner)
{
    AstNode* root = multiplicativeExpression(scanner);
    return parseLoop(scanner, multiplicativeExpression, root, {TokenType::PLUS, TokenType::MINUS});
}
AstNode* parseExpression(Scanner& scanner)
{
    return additiveExpression(scanner);

}