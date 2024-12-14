#include "parser_internal.hpp"
#include "parser_utils.hpp"
#define CONSUME_TOKEN(parser, type) (parser)->scanner->consume(type)
#define GET_TOKEN(parser) (parser)->scanner->getToken()
#define PEEK_TOKEN(parser) (parser)->scanner->peekToken()

using namespace std;
AstNode *parseExpression(ParserState *parser)
{
    return conditionalExpression(parser);
}

AstNode *conditionalExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::DOUBLE_PIPE};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* logOr = logOrExpression(parser);
    if(PEEK_TOKEN(parser).type != TokenType::QUESTION_MARK)
    {
        return logOr;
    }
    CONSUME_TOKEN(parser, TokenType::QUESTION_MARK);
    AstNode* expr = parseExpression(parser);
    CONSUME_TOKEN(parser, TokenType::COLON);
    AstNode* condExpr = conditionalExpression(parser);

    AstNode* root = ALLOCATE_NODE(parser);
    root->nodeType = NodeType::CONDITIONAL_EXPRESSION;
    root->children.push_back(logOr);
    root->children.push_back(expr);
    root->children.push_back(condExpr);
    return root;

}

AstNode *logOrExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::DOUBLE_PIPE};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = logAndExpression(parser);
    return parseLoop(parser, logAndExpression, root, types, len);
}

AstNode *logAndExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::DOUBLE_AMPRESAND};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = incOrExpression(parser);
    return parseLoop(parser, incOrExpression, root, types, len);
}

AstNode *incOrExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::PIPE};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = excOrExpression(parser);
    return parseLoop(parser, excOrExpression, root, types, len);
}

AstNode *excOrExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::CARET};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = AndExpression(parser);
    return parseLoop(parser, AndExpression, root, types, len);
}

AstNode *AndExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::AMPRESAND};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = equalityExpression(parser);
    return parseLoop(parser, equalityExpression, root, types, len);
}

AstNode *equalityExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = relationalExpression(parser);
    return parseLoop(parser, relationalExpression, root, types, len);
}

AstNode *relationalExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::GREATER, 
    TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = shiftExpression(parser);
    return parseLoop(parser, shiftExpression, root, types, len);
}

AstNode *shiftExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::L_SHIFT, TokenType::R_SHIFT};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = additiveExpression(parser);
    return parseLoop(parser, additiveExpression, root, types, len);
}

AstNode *additiveExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::PLUS, TokenType::MINUS};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = multiplicativeExpression(parser);
    return parseLoop(parser, multiplicativeExpression, root, types, len);
}

AstNode *multiplicativeExpression(ParserState *parser)
{
    static constexpr TokenType types[] =  {TokenType::STAR, TokenType::SLASH, TokenType::PERCENT};
    static constexpr uint64_t len = sizeof(types)/sizeof(TokenType);
    AstNode* root = castExpression(parser);
    return parseLoop(parser, castExpression, root, types, len);
}

AstNode *castExpression(ParserState *parser)
{
    return unaryExpression(parser);
}

AstNode *unaryExpression(ParserState *parser)
{
    static constexpr TokenType unaryOperators[] = { TokenType::PLUS_PLUS,
    TokenType::MINUS_MINUS, TokenType::BANG, TokenType::TILDE, TokenType::PLUS};
    static constexpr uint64_t len = sizeof(unaryOperators)/sizeof(TokenType);
    // no unary 
    if(!parser->scanner->currentTokenOneOf(unaryOperators, len))
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
