#include "../../include/frontend/parser.hpp"
#include "../../include/frontend/scanner.hpp"
using namespace std;

static AstNode* parseDeclarator(Scanner& scanner)
{
    if(!scanner.currentTokenOneOf({TokenType::IDENTIFIER}))
    {
        fprintf(stdout, "Unexpected token at line %d", scanner.popToken().line);
        exit(-1);
    }
    Token token = scanner.popToken();
    
    AstNode* Identifier = new AstNode{NodeType::IDENTIFIER, {}, NodeDataType::NONE, new string()};
    *((string*)Identifier->context.arbitraryData) = *(string*)token.context.data;
   return Identifier;
}

static AstNode* parseInitializer(Scanner& scanner)
{
    return assignmentExpression(scanner);
}

static AstNode* parseInitDeclarator(Scanner& scanner)
{
    AstNode* declarator = parseDeclarator(scanner);
    if(!scanner.match(TokenType::EQUAL))
    {
        return declarator;
    }

    AstNode* initializer = parseInitializer(scanner);

    AstNode* root = new AstNode{NodeType::DECLARATION_GLUE, {declarator, initializer}, NodeDataType::INFERED};
    return root;
}


AstNode* parseDeclaration(Scanner& scanner)
{

    NodeDataType dataType = transformToDataType(scanner, scanner.popToken());
    AstNode* root_parent = new AstNode{NodeType::DECLARATION, {}, dataType};
    
    if(scanner.match(TokenType::SEMICOLON))
    {
        return root_parent;
    }

    AstNode* init_declarator = parseInitDeclarator(scanner);

    root_parent->children.push_back(init_declarator);

    while (scanner.match(TokenType::COMMA))
    {
        root_parent->children.push_back(parseInitDeclarator(scanner));
    }
    scanner.consume(TokenType::SEMICOLON);
    return root_parent;
}