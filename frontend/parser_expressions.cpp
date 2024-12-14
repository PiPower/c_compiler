#include "parser_internal.hpp"
#include "parser_utils.hpp"

using namespace std;
#define ALLOCATE_NODE(parser) allocateNode((parser)->allocator)
#define GET_TOKEN(parser) (parser)->scanner->getToken()
#define PEEK_TOKEN(parser) (parser)->scanner->peekToken()


AstNode *parseExpression(ParserState *parser)
{
    return unaryExpression(parser);
}

AstNode *castExpression(ParserState *parser)
{
    return unaryExpression(parser);
}

AstNode *unaryExpression(ParserState *parser)
{
    static const TokenType unaryOperators[] = { TokenType::PLUS_PLUS,
    TokenType::MINUS_MINUS, TokenType::BANG, TokenType::TILDE, TokenType::PLUS};
    // no unary 
    if(!parser->scanner->currentTokenOneOf(unaryOperators, sizeof(TokenType)))
    {
        return primaryExpression(parser);
    }

    Token token = parser->scanner->getToken();
    if(token.type == TokenType::PLUS_PLUS || token.type == TokenType::MINUS_MINUS)
    {
        AstNode* root = ALLOCATE_NODE(parser);
        root->nodeType = token.type == TokenType::PLUS ? NodeType::PRE_INC : NodeType::PRE_DEC;
        root->children.push_back(unaryExpression(parser));
        return root;
    }
    AstNode* root = ALLOCATE_NODE(parser);
    root->nodeType = token.type == TokenType::BANG ? NodeType::LOG_NOT : NodeType::COMPLIMENT;
    root->nodeType = token.type == TokenType::PLUS ? NodeType::ADD : root->nodeType;
    root->children.push_back(castExpression(parser));
    return root;
}

AstNode *parseStatement(ParserState *parser)
{
    AstNode* root = parseExpression(parser);
    parser->scanner->consume(TokenType::SEMICOLON);
    return root;
}

AstNode *postfixExpression(ParserState *parser)
{
    return primaryExpression(parser);
}

AstNode *primaryExpression(ParserState *parser)
{
    Token token = parser->scanner->getToken();
    AstNode* root;
    switch (token.type)
    {
    case TokenType::IDENTIFIER:
        root = ALLOCATE_NODE(parser);
        root->nodeType = NodeType::IDENTIFIER;
        root->data = token.data;
        return root;
    case TokenType::CONSTANT:
        root = ALLOCATE_NODE(parser);
        root->nodeType = NodeType::CONSTANT;
        root->data = token.data;
        return root;
    case TokenType::L_PARENTHESES:
        root = parseExpression(parser);
        parser->scanner->consume(TokenType::R_PARENTHESES);
        return root;
    default:
        triggerParserError(parser, 0, "Unexpected token for primary expression \n");
        break;
    }

    return nullptr;
}
