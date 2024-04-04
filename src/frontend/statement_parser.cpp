#include "../../include/frontend/parser.hpp"


AstNode* parseWhileLoop(Scanner& scanner)
{
    scanner.consume(TokenType::WHILE);
    scanner.consume(TokenType::L_PARENTHESES);
    AstNode* expr = parseExpression(scanner);
    scanner.consume(TokenType::R_PARENTHESES);

    AstNode* body = parseStatement(scanner);

    return new AstNode{NodeType::WHILE_LOOP, {expr, body}, NodeDataType::INFERED};

}


AstNode* parseDoWhileLoop(Scanner& scanner)
{
    scanner.consume(TokenType::DO);
    AstNode* body = parseStatement(scanner);

    scanner.consume(TokenType::WHILE);
    scanner.consume(TokenType::L_PARENTHESES);
    AstNode* expr = parseExpression(scanner);
    scanner.consume(TokenType::R_PARENTHESES);

    return new AstNode{NodeType::DO_WHILE_LOOP, {expr, body}, NodeDataType::INFERED};
}

AstNode* parseForLoop(Scanner& scanner)
{
    AstNode* init_expr,*cond_expr, *update_expr ;
    scanner.consume(TokenType::FOR);
    scanner.consume(TokenType::L_PARENTHESES);
    if(scanner.match(TokenType::SEMICOLON)){init_expr = nullptr;}
    else if(scanner.currentTokenOneOf({TokenType::INT})){init_expr = parseDeclaration(scanner);}
    else 
    {
        init_expr = parseExpression(scanner);
        scanner.consume(TokenType::SEMICOLON);
    }

    if(scanner.match(TokenType::SEMICOLON)){cond_expr = nullptr;}
    else 
    {
        cond_expr = parseExpression(scanner);
        scanner.consume(TokenType::SEMICOLON);
    }

    if(scanner.match(TokenType::R_PARENTHESES)){update_expr = nullptr;}
    else 
    {
        update_expr = parseExpression(scanner);
        scanner.consume(TokenType::R_PARENTHESES);
    }


    AstNode* body = parseStatement(scanner);
    return new AstNode{NodeType::FOR_LOOP, {init_expr, cond_expr, update_expr, body}, NodeDataType::INFERED};
}

AstNode* parseIterationStatement(Scanner& scanner)
{
    switch (scanner.getCurrentToken().type)
    {
    case TokenType::WHILE:
        return parseWhileLoop(scanner);
    case TokenType::DO:
        return parseDoWhileLoop(scanner);
    case TokenType::FOR:
        return parseForLoop(scanner);
    default:
        fprintf(stdout, "unsupported iteration statement");
        exit(-1);
        break;
    }
    return nullptr;
}
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
    else
    {
        return parseStatement(scanner);
    }
}
AstNode *parseStatement(Scanner &scanner)
{
    if(scanner.currentTokenOneOf({TokenType::IF}))
    {
        return parseSelectionStatement(scanner);
    }
    else if(scanner.currentTokenOneOf({TokenType::WHILE, TokenType::DO, TokenType::FOR}))
    {
        return parseIterationStatement(scanner);
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