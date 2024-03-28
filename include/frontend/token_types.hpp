#ifndef TOKEN_TYPES
#define TOKEN_TYPES

enum class TokenType
{
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
// punctuators
    L_BRACKET,  R_BRACKET, L_PARENTHESES,  R_PARENTHESES, 
    L_BRACE, R_BRACE, DOT, ARROW, INCREMENT, DECREMENT, 
    AMPRESAND, STAR, PLUS, MINUS, TILDE, BANG, 
    SLASH, SEMICOLON,
};


enum class ConstantType
{
    INT_32
};

struct Token
{
    TokenType type;
    unsigned int line;
    void* data = nullptr; //other context dependent data
};

#endif