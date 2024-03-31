#ifndef PARSER_UTILS
#define PARSER_UTILS
#include "./scanner.hpp"

enum class NodeType
{
    IDENTIFIER,
    CONSTANT,
    DECLARATION,
    CONDITIONAL_EXPRESSION,
//basic math ops
    MULTIPLY, DIVIDE, DIVIDE_MODULO,
    ADD, SUBTRACT, L_SHIFT, R_SHIFT,
    AND,EXC_OR, OR, LOG_AND, LOG_OR, 
// comparisons
    LESS, GREATER, LESS_EQUAL, 
    GREATER_EQUAL, EQUAL, NOT_EQUAL,
// assignment nodes 
    ASSIGNMENT, MUL_ASSIGNMENT, DIV_ASSIGNMENT, MOD_ASSIGNMENT,
    ADD_ASSIGNMENT, SUBB_ASSIGNMENT, L_SHIFT_ASSIGNMENT, R_SHIFT_ASSIGNMENT,
    AND_ASSIGNMENT, EXC_OR_ASSIGNMENT, OR_ASSIGNMENT,
// misc
    EXPR_GLUE, DECLARATION_GLUE
};

enum class DataType
{
    NONE,
    INFERED,
    INT_64
};

typedef DataType NodeDataType;

NodeDataType transformToDataType(Scanner& scanner, Token token);
bool isTypeSpecifier(Token token);

NodeType tokenMathTypeToNodeType(const Token token);
NodeType assignementTokenToNodeType(const Token token);
#endif