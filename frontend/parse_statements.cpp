#include "parser_internal.hpp"

using namespace std;

AstNode* parseSelectionStatement(ParserState* parser)
{
    AstNode* root = new AstNode{NodeType::IF, {}};
    CONSUME_TOKEN(parser, TokenType::IF);
    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    AstNode* expr = parseExpression(parser);
    CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
    AstNode* body = parseStatement(parser);
    root->children.push_back(expr);
    root->children.push_back(body);
    while (TOKEN_MATCH(parser, TokenType::ELSE))
    {
        root->children.push_back(parseStatement(parser));
    }
    return root;
}


AstNode* parseIterationStatement(ParserState *parser)
{
    Token token = GET_TOKEN(parser);
    switch (token.type)
    {
    case TokenType::WHILE:
        return parseWhileLoop(parser);
    case TokenType::DO:
        return parseDoWhileLoop(parser);
    case TokenType::FOR:
        return parseForLoop(parser);
    default:
        fprintf(stdout, "unsupported iteration statement");
        exit(-1);
        break;
    }
    return nullptr;
}

AstNode *parseWhileLoop(ParserState *parser)
{
    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    AstNode* expr = parseExpression(parser);
    CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
    AstNode* body = parseStatement(parser);

    return new AstNode{NodeType::WHILE_LOOP, {expr, body}};
}

AstNode *parseDoWhileLoop(ParserState *parser)
{
    AstNode* body = parseStatement(parser);
    CONSUME_TOKEN(parser, TokenType::WHILE);
    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    AstNode* expr = parseExpression(parser);
    CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
    CONSUME_TOKEN(parser, TokenType::SEMICOLON);

    return new AstNode{NodeType::DO_WHILE_LOOP, {expr, body}};
}

// TODO add possibility of variable declaration within for loop
AstNode *parseForLoop(ParserState *parser)
{
    AstNode* init_expr = nullptr, *cond_expr = nullptr, *update_expr = nullptr ;
    CONSUME_TOKEN(parser, TokenType::L_PARENTHESES);
    if(!TOKEN_MATCH(parser, TokenType::SEMICOLON)) 
    {
        init_expr = parseExpression(parser);
        CONSUME_TOKEN(parser, TokenType::SEMICOLON);
    }

    if(!TOKEN_MATCH(parser, TokenType::SEMICOLON))
    {
        cond_expr = parseExpression(parser);
        CONSUME_TOKEN(parser, TokenType::SEMICOLON);
    }

    if(!TOKEN_MATCH(parser, TokenType::R_PARENTHESES))
    {
        update_expr = parseExpression(parser);
        CONSUME_TOKEN(parser, TokenType::R_PARENTHESES);
    }

    AstNode* body = parseStatement(parser);
    return new AstNode{NodeType::FOR_LOOP, {init_expr, cond_expr, update_expr, body}};
}

AstNode *parseStatement(ParserState *parser)
{
    AstNode* root;
    if(CURRENT_TOKEN_ON_OF(parser, {TokenType::END_OF_FILE}))
    {
        return nullptr;
    }  
    else if(CURRENT_TOKEN_ON_OF(parser, {TokenType::IF}))
    {
        return parseSelectionStatement(parser);
    }
    else if(CURRENT_TOKEN_ON_OF(parser, {TokenType::L_BRACE}))
    {
        SymbolTable* blockSymtab = new SymbolTable();
        blockSymtab->scopeLevel = parser->symtab->scopeLevel + 1;
        blockSymtab->parent = parser->symtab;
        parser->symtab = blockSymtab;
        root = parseCompoundStatement(parser);

        parser->symtab = parser->symtab->parent;
        return root;
    }
    else if(CURRENT_TOKEN_ON_OF(parser, {TokenType::WHILE, TokenType::FOR, TokenType::DO}))
    {
        return parseIterationStatement(parser);
    }
    else
    {
        root = parseExpression(parser);
    }
    parser->scanner->consume(TokenType::SEMICOLON);
    return root;
}

AstNode* parseCompoundStatement(ParserState* parser)
{
    AstNode* block = ALLOCATE_NODE(parser);
    block->data = (string*)parser->symtab;
    block->nodeType = NodeType::BLOCK;
    CONSUME_TOKEN(parser, TokenType::L_BRACE);
    while (PEEK_TOKEN(parser).type != TokenType::R_BRACE)
    {
        AstNode* root = parseDeclaration(parser);
        if(root && root != PARSER_SUCC)
        {
            vector<AstNode*> processed = processDeclarationTree(root, parser);
            block->children.insert(block->children.end(),processed.begin(), processed.end());
        }
        if(!root)
        {
            root = parseStatement(parser);
            block->children.push_back(root);
        }
    };
    CONSUME_TOKEN(parser, TokenType::R_BRACE);
    return block;
}