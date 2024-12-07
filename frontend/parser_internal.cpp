#include "parser_internal.hpp"
#include <stdarg.h>

AstNode *parseExpression(ParserState *parser)
{
    return nullptr;
}

AstNode *parseStatement(ParserState *parser)
{
    AstNode* root = parseExpression(parser);
    parser->scanner->consume(TokenType::SEMICOLON);
    return root;
}

AstNode* primaryExpression(ParserState *parser)
{
    Token token = parser->scanner->getToken();
    AstNode* root;
    switch (token.type)
    {
    case TokenType::IDENTIFIER:
        root = new AstNode{NodeType::IDENTIFIER, {}, token.data};
        return root;
    case TokenType::CONSTANT:
        root = new AstNode{NodeType::CONSTANT, {}, token.data};
        return root;
    case TokenType::L_PARENTHESES:
        root = parseExpression(parser);
        parser->scanner->consume(TokenType::R_PARENTHESES);
        return root;
    default:
        triggerParserError(parser, 0, "Unexpected token for primary expression \n");
        break;
    }
}


void triggerParserError(ParserState* parser, int value, const char* format, ...)
{
    parser->errCode = 1;
    
    va_list args;
    va_start(args, format);
    int code = vsnprintf(parser->errorMessage, parser->errorMessageLen, format, args);
    va_end(args);

    longjmp(parser->jmpBuff, value);
}
