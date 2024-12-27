#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H
#include "scanner.hpp"
#include "../symbol_table.hpp"
// math ops  
//   AMPRESAND, STAR, PLUS, MINUS, TILDE, BANG, SLASH, 
//   PERCENT, LESS, GREATER,  L_SHIFT,
//    R_SHIFT, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, BANG_EQUAL,
//   PIPE, CARET, DOUBLE_AMPRESAND, DOUBLE_PIPE, QUESTION_MARK,
//    PLUS_PLUS, MINUS_MINUS,

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
    AND, EXC_OR, OR, COMPLIMENT,
// prefix ops
    PRE_INC, POST_INC, PRE_DEC, POST_DEC,
//logical ops    
    LOG_AND, LOG_OR, LOG_NOT,
// comparisons
    LESS, GREATER, LESS_EQUAL, 
    GREATER_EQUAL, EQUAL, NOT_EQUAL,
// assignment nodes 
    ASSIGNMENT, MUL_ASSIGNMENT, DIV_ASSIGNMENT, MOD_ASSIGNMENT,
    ADD_ASSIGNMENT, SUB_ASSIGNMENT, L_SHIFT_ASSIGNMENT, R_SHIFT_ASSIGNMENT,
    AND_ASSIGNMENT, EXC_OR_ASSIGNMENT, OR_ASSIGNMENT,
// function
    FUNCTION_DECL, FUNCTION_DEF, ARGS_EXPR_LIST,
    FUNCTION_CALL, PARAMETER_LIST, IDENTIFIER_LIST,
// statement
    BLOCK, IF, ELSE,
// misc
    NODE_EMPTY, DECLARATION_LIST, TYPE_PLACEHOLDER, INITALIZER
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
AstNode* fillSymtab(AstNode* root);
NodeType tokenMathTypeToNodeType(const Token& token);

static inline NodeType assignementTokenToNodeType(const Token& token)
{
    return (NodeType) ((int)token.type - (int)TokenType::EQUAL + (int)NodeType::ASSIGNMENT) ;
}
//fills and checks symbol table
// removes nodes that are more abstact
std::vector<AstNode*> processDeclarationTree(AstNode* root, ParserState* parser);
AstNode* processVariable(AstNode *root, ParserState *parser);
AstNode* processFunction(AstNode* root,ParserState* parser);
#endif