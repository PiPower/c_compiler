#include "parser_internal.hpp"

AstNode *parseStatement(ParserState *parser)
{
    if(PEEK_TOKEN(parser).type == TokenType::END_OF_FILE)
    {
        return nullptr;
    }
    AstNode* root = parseExpression(parser);
    parser->scanner->consume(TokenType::SEMICOLON);
    return root;
}

AstNode* parseCompoundStatement(ParserState* parser)
{
    AstNode* block = ALLOCATE_NODE(parser);
    block->nodeType = NodeType::BLOCK;
    CONSUME_TOKEN(parser, TokenType::L_BRACE);
    while (PEEK_TOKEN(parser).type != TokenType::R_BRACE)
    {
        AstNode* root = parseDeclaration(parser);
        if(!root)
        {
            root = parseStatement(parser);
        }
        if(root && root != PARSER_SUCC)
        {
            block->children.push_back(root);
        }
    };
    CONSUME_TOKEN(parser, TokenType::R_BRACE);
    return block;
}