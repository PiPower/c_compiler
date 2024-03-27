#ifndef TOKEN_TYPES
#define TOKEN_TYPES

enum class TokenType
{
    EOF,
    KEYWORD,
    IDENTIFIER,
    CONSTANT,
    STRING_LITERAL,
    PUNCTUATOR
};

enum class KeywordType
{
    AUTO,
    BREAK,
    CASE,
    CHAR,
    CONST,
    CONTINUE,
    DEFAULT,
    DO,
    DOUBLE,
    ELSE,
    ENUM,
    EXTERN,
    FLOAT,
    FOR,
    GOTO,
    IF,
    INLINE,
    INT,
    LONG,
    REGISTER,
    RESTRICT,
    RETURN,
    SHORT,
    SIGNED,
    SIZEOF,
    STATIC,
    STRUCT,
    SWITCH,
    TYPEDEF,
    UNION,
    UNSIGNED,
    VOID,
    VOLATILE,
    WHILE,
    _BOOL,
    _COMPLEX,
    _IMAGINARY,
};

enum class PunctuatorType
{
    L_BRACKET, 
    R_BRACKET, 
    L_PARENTHESES, 
    R_PARENTHESES, 
    L_BRACE, 
    R_BRACE, 
    DOT, ARROW,
    INCREMENT, 
    DECREMENT, 
    AMPRESAND, 
    STAR, PLUS, 
    MINUS, 
    TILDE,
    BANG, 
    SLASH,
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
        KeywordType kType; //if keyword
        PunctuatorType pType; //if punctuator
        ConstantType cType; //if constant
    };
    char* data = nullptr; //other context dependent data
};

#endif