#include "../../include/frontend/parser.hpp"

using namespace std;
vector<AstNode*> parse(Scanner& scanner)
{
    Token token = scanner.getCurrentToken();
    while (token.type != TokenType::END_OF_FILE)
    {
        if(isTypeSpecifier(token))
        {
            parseDeclaration(scanner, token);
        }
    }
    
}


AstNode* parseDeclaration(Scanner& scanner, Token type)
{
    NodeDataType dataType = transformToDataType(scanner, type);
    AstNode* root_parent = new AstNode{NodeType::DECLARATION, {}, dataType};

    AstNode* root = root_parent;
    Token token = scanner.getCurrentToken();
    while (isTypeSpecifier(token))
    {
        dataType = transformToDataType(scanner, token);
        AstNode* child = new AstNode{NodeType::DECLARATION, {}, dataType};
        root->children.push_back(child);
        root = child;
    }
    
    if(token.type == TokenType::SEMICOLON)
    {
        return root_parent;
    }
    root->children.push_back( parseDeclarator(scanner, token) );

    scanner.consume(TokenType::SEMICOLON);
    return root_parent;
}

AstNode* parseDeclarator(Scanner& scanner, Token token)
{
    return parseDirectDeclarator(scanner, token);
}

AstNode* parseDirectDeclarator(Scanner& scanner, Token token)
{
    if(token.type != TokenType::IDENTIFIER)
    {
        fprintf(stdout, "unexpected token at line %d", token.line);
        exit(-1);
    }

    return new AstNode{NodeType::IDENTIFIER, {}, NodeDataType::NONE };
}