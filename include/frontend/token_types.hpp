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
    AUTO, BREAK, CASE, CONST, CONTINUE,
    DEFAULT, DO, ELSE, ENUM, EXTERN, FOR,
    GOTO, IF, INLINE, REGISTER, RESTRICT,
    RETURN, SIZEOF, STATIC, STRUCT, SWITCH,
    TYPEDEF, UNION, VOLATILE, WHILE,
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