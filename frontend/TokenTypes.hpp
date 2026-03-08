#pragma once
#include <inttypes.h>
#include "../utils/FileManager.hpp"

#define TOKEN_LIST \
    X(eof) \
    X(none) \
    X(identifier) \
    X(numeric_constant) \
    X(character_literal) \
    X(l_character_literal) \
    X(string_literal) \
    X(l_string_literal) \
    /* keywords */ \
    KW(break) \
    KW(case) \
    KW(continue) \
    KW(do) \
    KW(else) \
    KW(if) \
    KW(for) \
    KW(return) \
    KW(while) \
    KW(typedef) \
    KW(extern) \
    KW(static) \
    KW(auto) \
    KW(register) \
    KW(enum) \
    KW(sizeof) \
    KW(default) \
    KW(switch) \
    KW(inline) \
    KW(goto) \
    /* type keywords */ \
    KW(unsigned) \
    KW(int) \
    KW(long) \
    KW(float) \
    KW(double) \
    KW(imaginary) \
    KW(signed) \
    KW(char) \
    KW(short) \
    KW(void) \
    KW(bool) \
    KW(complex) \
    KW(struct) \
    KW(union) \
    /* type qualifiers */ \
    KW(const) \
    KW(volatile) \
    KW(restrict) \
    /* separators */ \
    X(l_bracket) \
    X(r_bracket) \
    X(l_parentheses) \
    X(r_parentheses) \
    X(l_brace) \
    X(r_brace) \
    /* math ops */ \
    X(ampersand) \
    X(star) \
    X(plus) \
    X(minus) \
    X(tilde) \
    X(bang) \
    X(slash) \
    X(percent) \
    X(less) \
    X(greater) \
    X(l_shift) \
    X(r_shift) \
    X(less_equal) \
    X(greater_equal) \
    X(equal_equal) \
    X(bang_equal) \
    X(pipe) \
    X(caret) \
    X(double_ampersand) \
    X(double_pipe) \
    X(question_mark) \
    X(plus_plus) \
    X(minus_minus) \
    /* assignment */ \
    X(equal) \
    X(star_equal) \
    X(slash_equal) \
    X(percent_equal) \
    X(plus_equal) \
    X(minus_equal) \
    X(l_shift_equal) \
    X(r_shift_equal) \
    X(ampresand_equal) \
    X(caret_equal) \
    X(pipe_equal) \
    /* misc */ \
    X(colon) \
    X(comma) \
    X(semicolon) \
    X(dot) \
    X(arrow) \
    X(ellipsis) \
    X(new_line) \
    X(hash) \
    X(d_hash) \
    X(comment) \
    X(line_splice) \
    /* preprocessor keywords */ \
    PP(include) \
    PP(define) \
    PP(ifdef) \
    PP(ifndef) \
    PP(elif) \
    PP(endif) \
    PP(line) \
    PP(error) \
    PP(pragma) \
    PP(undef) \
    PP(defined)
    
    enum YOLO
    {
        x, y,
    };

namespace TokenType
{
    enum Type : uint16_t
    {
       
        #define X(name) name,
        #define KW(name) kw_##name,
        #define PP(name) pp_##name,
        
        TOKEN_LIST

        #undef PP
        #undef KW
        #undef X
    };


    constexpr const char* tokenStr(TokenType::Type type)
    {
        switch (type)
        {
            #define CASE(name) case TokenType::name: return #name;
            #define X(name) CASE(name)
            #define KW(name) CASE(kw_##name)
            #define PP(name) CASE(pp_##name)
            
            TOKEN_LIST

            #undef PP
            #undef KW
            #undef X
            #undef CASE

        default:
            break;
        }
        return nullptr;
    }
}

struct SourceLocation
{
    SourceLocation() = default;
    
    SourceLocation(size_t id,  int64_t offset, int64_t line, int64_t len) :
    id(FILE_ID{id}), offset(offset), line(line), len(len)
    {}

    FILE_ID id;
    int64_t offset;
    int64_t line;
    int64_t len;
};

struct Token
{
    TokenType::Type type;
    SourceLocation location;
    // hints
    uint16_t skippedHorizWhitespace : 1; // prepended by whitespace
    uint16_t isFloat : 1; // is numeric constant float
    uint16_t isHex : 1; // is numeric written in hexadecimal notation
    uint16_t isDec : 1; // is numeric written in decimal notation
    uint16_t isBin : 1; // is numeric written in binary notation
    uint16_t isOct : 1; // is numeric written in octal notation
    uint16_t hasE : 1; // does numeric contain E
    uint16_t hasP : 1; // does numeric contain P
};

template<typename... Args>
static bool IsTokenOneOf(const Token* token, Args&&... args)
{
    return ((token->type == args) || ...);
}

#undef TOKEN_LIST