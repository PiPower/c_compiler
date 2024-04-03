#include "../../include/frontend/parser.hpp"

AstNode* parseSelectionStatement(Scanner& scanner)
{
    AstNode* root = new AstNode{NodeType::IF, {}, NodeDataType::INFERED};
    scanner.consume(TokenType::IF);

    scanner.consume(TokenType::L_PARENTHESES);
    AstNode* expr = parseExpression(scanner);
    scanner.consume(TokenType::R_PARENTHESES);

    AstNode* body = parseStatementAndDeclaration(scanner);

    root->children.push_back(expr);
    root->children.push_back(body);

    while (scanner.match(TokenType::ELSE))
    {
        root->children.push_back(parseStatement(scanner));
    }
    return root;
}

AstNode* parseCompoundStatement(Scanner& scanner)
{
    scanner.consume(TokenType::L_BRACE);
    AstNode* block = new AstNode{NodeType::BLOCK, {}, NodeDataType::NONE};
    while (scanner.getCurrentToken().type != TokenType::R_BRACE)
    {
        AstNode* child = parseStatementAndDeclaration(scanner);
        block->children.push_back(child);
    }
    scanner.consume(TokenType::R_BRACE);
    return block;
}

AstNode *parseStatementAndDeclaration(Scanner &scanner)
{
    if(isTypeSpecifier(scanner.getCurrentToken()))
    {
        return parseDeclaration(scanner);
    }
    else if(scanner.currentTokenOneOf({TokenType::IF}))
    {
        return parseSelectionStatement(scanner);
    }
    else if(scanner.currentTokenOneOf({TokenType::L_BRACE}) )
    {
        return parseCompoundStatement(scanner);
    }
    else if (scanner.match(TokenType::SEMICOLON))
    {
        return nullptr;
    }
    else
    {
        AstNode* root = parseExpression(scanner);
        scanner.consume(TokenType::SEMICOLON);
        return root;
    }
}
AstNode *parseStatement(Scanner &scanner)
{
    if(scanner.currentTokenOneOf({TokenType::IF}))
    {
        return parseSelectionStatement(scanner);
    }
    else if(scanner.currentTokenOneOf({TokenType::L_BRACE}) )
    {
        return parseCompoundStatement(scanner);
    }
    else if (scanner.match(TokenType::SEMICOLON))
    {
        return nullptr;
    }
    else
    {
        AstNode* root = parseExpression(scanner);
        scanner.consume(TokenType::SEMICOLON);
        return root;
    }
}