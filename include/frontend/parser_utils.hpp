#ifndef PARSER_UTILS
#define PARSER_UTILS
#include "./scanner.hpp"

enum class NodeType
{
    IDENTIFIER,
    DECLARATION
};

enum class NodeDataType
{
    NONE,
    INT_32
};


NodeDataType transformToDataType(Scanner& scanner, Token token);
bool isTypeSpecifier(Token& token);


#endif