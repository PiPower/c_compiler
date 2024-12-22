#ifndef TOKEN_H
#define TOKEN_H
#include <string>

enum class TokenType
{
    NONE,
    END_OF_FILE,
    IDENTIFIER,
    CONSTANT,
// keywords
    BREAK, CASE, CONTINUE,
    DO, ELSE, IF, FOR,
    RETURN, WHILE, TYPE,
// type qualifier
    CONST, VOLATILE,
// separators
    L_BRACKET,  R_BRACKET, L_PARENTHESES,  R_PARENTHESES, 
    L_BRACE, R_BRACE,
// math ops  
    AMPRESAND, STAR, PLUS, MINUS, TILDE, BANG, SLASH, 
    PERCENT, LESS, GREATER,  L_SHIFT,
    R_SHIFT, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, BANG_EQUAL,
    PIPE, CARET, DOUBLE_AMPRESAND, DOUBLE_PIPE, QUESTION_MARK,
    PLUS_PLUS, MINUS_MINUS,
// assignemnt types
    EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL, PLUS_EQUAL, 
    MINUS_EQUAL, L_SHIFT_EQUAL, R_SHIFT_EQUAL, AMPRESAND_EQUAL,
    CARET_EQUAL, PIPE_EQUAL,
// miscallenous
    COLON, COMMA , SEMICOLON, DOT, 
// for internal usage
    COMMENT
};

struct Token
{
    TokenType type;
    unsigned int line;
    // if type is identifier, constant type:
    // data points to std::string
    char* data;
};

extern const char* tokenTypeString[];
#endif