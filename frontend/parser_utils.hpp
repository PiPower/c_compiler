#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H
#include "scanner.hpp"

enum class NodeType
{
    NONE, 
    IDENTIFIER,
    CONSTANT,
    DECLARATION,
    CONDITIONAL_EXPRESSION,
//basic math ops
    MULTIPLY, DIVIDE, DIVIDE_MODULO,
    ADD, SUBTRACT, L_SHIFT, R_SHIFT,
    AND, EXC_OR, OR, NOT, COMPLIMENT,
// prefix ops
    PRE_INC, POST_INC, PRE_DEC, POST_DEC,
//logical ops    
    LOG_AND, LOG_OR, LOG_NOT,
// comparisons
    LESS, GREATER, LESS_EQUAL, 
    GREATER_EQUAL, EQUAL, NOT_EQUAL,
// assignment nodes 
    ASSIGNMENT, MUL_ASSIGNMENT, DIV_ASSIGNMENT, MOD_ASSIGNMENT,
    ADD_ASSIGNMENT, SUBB_ASSIGNMENT, L_SHIFT_ASSIGNMENT, R_SHIFT_ASSIGNMENT,
    AND_ASSIGNMENT, EXC_OR_ASSIGNMENT, OR_ASSIGNMENT,
// misc
    EXPR_GLUE, IF, BLOCK, WHILE_LOOP,
    DO_WHILE_LOOP, FOR_LOOP,FUNCTION_PARAMS,
    DECLARATION_VARIABLE, DEFINITION_VARIABLE,
    FUNCTION_DECLARATION, FUNCTION_DEFINITION,
    FUNCTION_CALL, RETURN
};

struct ParserState;
struct AstNode;


typedef AstNode* ( *parseFunctionPtr)(ParserState* parser);

void triggerParserError(ParserState* parser,
                         int value, 
                         const char* format, ...);
AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction,
                    AstNode* root, 
                    const std::vector<TokenType>& types);
AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction,
                    AstNode* root, 
                    const TokenType* types,
                    const uint64_t typesCount);
NodeType tokenMathTypeToNodeType(const Token& token);

#endif