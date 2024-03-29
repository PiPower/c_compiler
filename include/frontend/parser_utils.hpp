#ifndef PARSER_UTILS
#define PARSER_UTILS
#include "./scanner.hpp"

enum class NodeType
{
    IDENTIFIER,
    CONSTANT,
    DECLARATION,
    ASSIGNMENT,
    CONDITIONAL_EXPRESSION,
//basic math ops
    MULTIPLY, DIVIDE, DIVIDE_MODULO,
    ADD, SUBTRACT, L_SHIFT, R_SHIFT,
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    EQUAL, NOT_EQUAL, AND,
    EXC_OR, OR, LOG_AND, LOG_OR, 
};

enum class NodeDataType
{
    NONE,
    INFERED,
    INT_32
};


NodeDataType transformToDataType(Scanner& scanner, Token token);
bool isTypeSpecifier(Token& token);

NodeType tokenMathTypeToNodeType(const Token token);

#endif