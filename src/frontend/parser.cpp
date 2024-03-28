#include "../../include/frontend/parser.hpp"

using namespace std;
vector<AstNode*> parse(Scanner& scanner)
{
    vector<AstNode*> instructions;
    Token token = scanner.getCurrentToken();

    while (token.type != TokenType::END_OF_FILE)
    {
        if(isTypeSpecifier(token))
        {
            instructions.push_back(parseDeclaration(scanner));
        }
    }
    
    return instructions;
}


AstNode* parseDeclaration(Scanner& scanner)
{
    NodeDataType dataType = transformToDataType(scanner, scanner.popToken());
    AstNode* root_parent = new AstNode{NodeType::DECLARATION, {}, dataType};

    AstNode* root = root_parent;
    Token token = scanner.getCurrentToken();
    while (isTypeSpecifier(token))
    {
        dataType = transformToDataType(scanner, scanner.popToken());
        AstNode* child = new AstNode{NodeType::DECLARATION, {}, dataType};
        root->children.push_back(child);
        root = child;

        token = scanner.getCurrentToken();
    }
    
    if(token.type == TokenType::SEMICOLON)
    {
        return root_parent;
    }
    root->children.push_back( parseDeclarator(scanner) );

    scanner.consume(TokenType::SEMICOLON);
    return root_parent;
}

AstNode* parseDeclarator(Scanner& scanner)
{
    return parseDirectDeclarator(scanner);
}

AstNode* parseDirectDeclarator(Scanner& scanner)
{
    Token token = scanner.popToken();
    if(token.type != TokenType::IDENTIFIER)
    {
        fprintf(stdout, "unexpected token at line %d", token.line);
        exit(-1);
    }

    return new AstNode{NodeType::IDENTIFIER, {}, NodeDataType::NONE };
}