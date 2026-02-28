#pragma once
#include <inttypes.h>

namespace TokenType
{
    enum Type : uint16_t
    {
        eof,
    // keywords
        kw_break, kw_case, kw_continue, kw_do, kw_else, kw_if, kw_for,
        kw_return, kw_while, kw_typedef, kw_extern, kw_static, 
        kw_auto, kw_register, kw_enum, kw_sizeof, kw_default,
        kw_switch, kw_inline,
    // type keywords
        kw__unsigned, kw_int, kw_long, kw_float, kw_double, kw_imaginary,
        kw_signed, kw_char, kw_short, kw_void, kw_bool, kw_complex, 
         kw_struct, kw_union, 
    // type qualifier keywords
         kw_const,  kw_volatile,  kw_restrict,
    // separators
        l_bracket,  r_bracket, l_parentheses,  r_parentheses, 
        l_brace, r_brace,
    // math ops  
        ampersand, star, plus, minus, tilde, bang, slash, 
        percent, less, greater,  l_shift,
        r_shift, less_equal, greater_equal, equal_equal, bang_equal,
        pipe, caret, double_ampersand, double_pipe, question_mark,
        plus_plus, minus_minus,
    // assignemnt types
        equal, star_equal, slash_equal, percent_equal, plus_equal, 
        minus_equal, l_shift_equal, r_shift_equal, ampresand_equal,
        caret_equal, pipe_equal,
    // miscallenous
        colon, comma , semicolon, dot, arrow, ellipsis,
    // for internal usage
        comment
    };
}
