#include "../../include/frontend/parser_utils.hpp"


NodeDataType transformToDataType(Scanner& scanner, Token token)
{
    if(token.type == TokenType::INT) return NodeDataType::INT_32;
}

bool isTypeSpecifier(Token &token)
{
    return (int)token.type >= (int)TokenType::VOID && (int)token.type <= (int)TokenType::_COMPLEX  ;
}