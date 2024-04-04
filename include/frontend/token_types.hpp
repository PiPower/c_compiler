#ifndef TOKEN_TYPES
#define TOKEN_TYPES

enum class TokenType
{
    NONE,
    END_OF_FILE,
    IDENTIFIER,
    CONSTANT,
    STRING_LITERAL,
// keywords
    BREAK, CASE, CONTINUE,
    DEFAULT, DO, ELSE,
    GOTO, IF, SWITCH, FOR,
    RETURN, SIZEOF, STRUCT,
    UNION, WHILE, ENUM,
// type qualifier
    RESTRICT, CONST, VOLATILE,
// function specifier
    INLINE,
// storage specifiers 
    REGISTER, EXTERN, STATIC, AUTO, TYPEDEF,
//  keywords types
    VOID, CHAR, SHORT, INT, LONG, FLOAT, 
    DOUBLE, SIGNED, UNSIGNED, _BOOL, _COMPLEX,
// separators
    L_BRACKET,  R_BRACKET, L_PARENTHESES,  R_PARENTHESES, 
    L_BRACE, R_BRACE,
// math ops  
    AMPRESAND, STAR, PLUS, MINUS, TILDE, BANG, SLASH, 
    PERCENT, LESS, GREATER,  L_SHIFT,
    R_SHIFT, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, BANG_EQUAL,
    PIPE, CARET, DOUBLE_AMPRESAND, DOUBLE_PIPE, QUESTION_MARK,
// assignemnt types
    EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL, PLUS_EQUAL, 
    MINUS_EQUAL, L_SHIFT_EQUAL, R_SHIFT_EQUAL, AMPRESAND_EQUAL,
    CARET_EQUAL, PIPE_EQUAL,
// miscallenous
     COLON, COMMA , SEMICOLON, DOT, ARROW, INCREMENT, DECREMENT,
};


enum class ConstantType
{
    INT_32
};

struct Token
{
    TokenType type;
    unsigned int line;
    union
    {
    void* data; //other context dependent data
    int int_32;
    } context;
};

#endif