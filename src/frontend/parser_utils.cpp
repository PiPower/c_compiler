#include "../../include/frontend/parser_utils.hpp"


NodeDataType transformToDataType(Scanner& scanner, Token token)
{
    if(token.type == TokenType::INT) return NodeDataType::INT_32;
    return NodeDataType::NONE;
}

bool isTypeSpecifier(Token &token)
{
    return (int)token.type >= (int)TokenType::VOID && (int)token.type <= (int)TokenType::_COMPLEX  ;
}

NodeType tokenMathTypeToNodeType(const Token token)
{
    switch (token.type)
    {
    case TokenType::PLUS :
        return NodeType::ADD;
    case TokenType::SLASH :
        return NodeType::DIVIDE;
    case TokenType::PERCENT :
        return NodeType::DIVIDE_MODULO;
    case TokenType::STAR :
        return NodeType::MULTIPLY;
    case TokenType::L_SHIFT :
        return NodeType::L_SHIFT;
    case TokenType::R_SHIFT :
        return NodeType::R_SHIFT;
    }

    fprintf(stdout, "unexpected token at line %d", token.line);
    exit(-1);
}