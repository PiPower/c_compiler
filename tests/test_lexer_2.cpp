#include <vector>
#include <cstddef>
#include <iostream>
#include "../frontend/Lexer.hpp"
extern std::vector<TokenType::Type> tokens;

int main()
{
    const char* path = "examples/lexer_stress_test.c";
    const char* arr[] = {path};
    FileManager fileManager({path}, {29});
    FILE_STATE main;
    fileManager.GetFileState("examples/lexer_stress_test.c", 29, &main);
    CompilationOpts opts(1, (const char**)arr);
    Lexer lexer(main, &fileManager, &opts);

    Token tok;
    size_t i = 0;
    do{
        lexer.Lex(&tok);

        TokenType::Type expectedToken = tokens[i];

        if(tok.type != expectedToken)
        {
            std::cout << i << std::endl;
            exit(-1);
        }
        i++;

    }while (tok.type != TokenType::eof);

    
}

std::vector<TokenType::Type> tokens= {
 
        // /* ... */
    TokenType::comment,
    TokenType::new_line,

    // #include <stdio.h>
    TokenType::hash,
    TokenType::pp_include,
    TokenType::less,
    TokenType::identifier,
    TokenType::dot,
    TokenType::identifier,
    TokenType::greater,
    TokenType::new_line,

    // #include <stdlib.h>
    TokenType::hash,
    TokenType::pp_include,
    TokenType::less,
    TokenType::identifier,
    TokenType::dot,
    TokenType::identifier,
    TokenType::greater,
    TokenType::new_line,

    // #include <stdint.h>
    TokenType::hash,
    TokenType::pp_include,
    TokenType::less,
    TokenType::identifier,
    TokenType::dot,
    TokenType::identifier,
    TokenType::greater,
    TokenType::new_line,

    // #define PI 3.141592653589793
    TokenType::hash,
    TokenType::pp_define,
    TokenType::identifier,
    TokenType::numeric_constant,
    TokenType::new_line,

    // #define SQUARE(x) ((x) * (x))
    TokenType::hash,
    TokenType::pp_define,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::l_parentheses,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::star,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::r_parentheses,
    TokenType::new_line,

    // #define STR(x) #x
    TokenType::hash,
    TokenType::pp_define,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::hash,
    TokenType::identifier,
    TokenType::new_line,

    // #define CONCAT(a,b) a##b
    TokenType::hash,
    TokenType::pp_define,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::comma,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::identifier,
    TokenType::d_hash,
    TokenType::identifier,
    TokenType::new_line,

    // #define MULTILINE_MACRO(a, b) \
    TokenType::hash,
    TokenType::pp_define,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::comma,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::line_splice,
    TokenType::new_line,

    // ((a) > (b) ? (a) : (b))
    TokenType::l_parentheses,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::greater,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::question_mark,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::colon,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::r_parentheses,
    TokenType::new_line,

    // // Single-line comment
    TokenType::comment,
    TokenType::new_line,

    // /* Multi-line
    //    comment test */
    TokenType::comment,
    TokenType::new_line,

    // typedef unsigned long ulong_t;
    TokenType::kw_typedef,
    TokenType::kw__unsigned,
    TokenType::kw_long,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // enum Color {
    TokenType::kw_enum,
    TokenType::identifier,
    TokenType::l_brace,
    TokenType::new_line,

    // RED = 1,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::new_line,

    // GREEN,
    TokenType::identifier,
    TokenType::comma,
    TokenType::new_line,

    // BLUE = 10,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::new_line,

    // };
    TokenType::r_brace,
    TokenType::semicolon,
    TokenType::new_line,

    // struct Point {
    TokenType::kw_struct,
    TokenType::identifier,
    TokenType::l_brace,
    TokenType::new_line,

    // int x;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // int y;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // };
    TokenType::r_brace,
    TokenType::semicolon,
    TokenType::new_line,

    // union Data {
    TokenType::kw_union,
    TokenType::identifier,
    TokenType::l_brace,
    TokenType::new_line,

    // int i;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // float f;
    TokenType::kw_float,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // char str[20];
    TokenType::kw_char,
    TokenType::identifier,
    TokenType::l_bracket,
    TokenType::numeric_constant,
    TokenType::r_bracket,
    TokenType::semicolon,
    TokenType::new_line,

    // };
    TokenType::r_brace,
    TokenType::semicolon,
    TokenType::new_line,

    // static inline int add(int a, int b) {
    TokenType::kw_static,
    TokenType::kw_inline,
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::comma,
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // return a + b;
    TokenType::kw_return,
    TokenType::identifier,
    TokenType::plus,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // }
    TokenType::r_brace,
    TokenType::new_line,

    // int global_var = 42;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // const volatile int cv_var = 100;
    TokenType::kw_const,
    TokenType::kw_volatile,
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // void function_pointer_test(void) {
    TokenType::kw_void,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::kw_void,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // int (*func_ptr)(int, int) = add;
    TokenType::kw_int,
    TokenType::l_parentheses,
    TokenType::star,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::l_parentheses,
    TokenType::kw_int,
    TokenType::comma,
    TokenType::kw_int,
    TokenType::r_parentheses,
    TokenType::equal,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // int result = func_ptr(3, 4);
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::numeric_constant,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // printf("Result: %d\n", result);
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::string_literal,
    TokenType::comma,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // }
    TokenType::r_brace,
    TokenType::new_line,

    // int main(void) {
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::kw_void,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // comment
    TokenType::comment,
    TokenType::new_line,

    // int dec = 123;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // int oct = 0123;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // int hex = 0x1A3F;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // unsigned int u = 123u;
    TokenType::kw__unsigned,
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // long l = 1234567890L;
    TokenType::kw_long,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // long long ll = 123456789012345LL;
    TokenType::kw_long,
    TokenType::kw_long,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // comment
    TokenType::comment,
    TokenType::new_line,

    // float f1 = 3.14f;
    TokenType::kw_float,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // double d1 = 2.71828;
    TokenType::kw_double,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // double d2 = 1.23e-4;
    TokenType::kw_double,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // double d3 = 0x1.1p+2;
    TokenType::kw_double,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // comment
    TokenType::comment,
    TokenType::new_line,

    // char c1 = 'a';
    TokenType::kw_char,
    TokenType::identifier,
    TokenType::equal,
    TokenType::character_literal,
    TokenType::semicolon,
    TokenType::new_line,

    // char c2 = '\n';
    TokenType::kw_char,
    TokenType::identifier,
    TokenType::equal,
    TokenType::character_literal,
    TokenType::semicolon,
    TokenType::new_line,

    // char c3 = '\x41';
    TokenType::kw_char,
    TokenType::identifier,
    TokenType::equal,
    TokenType::character_literal,
    TokenType::semicolon,
    TokenType::new_line,

    // comment
    TokenType::comment,
    TokenType::new_line,

    // char str1[] = "Hello, World!";
    TokenType::kw_char,
    TokenType::identifier,
    TokenType::l_bracket,
    TokenType::r_bracket,
    TokenType::equal,
    TokenType::string_literal,
    TokenType::semicolon,
    TokenType::new_line,

    // char str2[] = "Escaped chars: ...";
    TokenType::kw_char,
    TokenType::identifier,
    TokenType::l_bracket,
    TokenType::r_bracket,
    TokenType::equal,
    TokenType::string_literal,
    TokenType::semicolon,
    TokenType::new_line,

    // char str3[] = "Concatenated " "string literal";
    TokenType::kw_char,
    TokenType::identifier,
    TokenType::l_bracket,
    TokenType::r_bracket,
    TokenType::equal,
    TokenType::string_literal,
    TokenType::string_literal,
    TokenType::semicolon,
    TokenType::new_line,

    // struct Point p = { .x = 10, .y = 20 };
    TokenType::kw_struct,
    TokenType::identifier,
    TokenType::identifier,
    TokenType::equal,
    TokenType::l_brace,
    TokenType::dot,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::dot,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::r_brace,
    TokenType::semicolon,
    TokenType::new_line,

    // union Data data;
    TokenType::kw_union,
    TokenType::identifier,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // data.f = 3.14f;
    TokenType::identifier,
    TokenType::dot,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // int arr[5] = { [0] = 1, [3] = 4 };
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::l_bracket,
    TokenType::numeric_constant,
    TokenType::r_bracket,
    TokenType::equal,
    TokenType::l_brace,
    TokenType::l_bracket,
    TokenType::numeric_constant,
    TokenType::r_bracket,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::l_bracket,
    TokenType::numeric_constant,
    TokenType::r_bracket,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::r_brace,
    TokenType::semicolon,
    TokenType::new_line,

    // int matrix[2][2] = {{1,2},{3,4}};
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::l_bracket,
    TokenType::numeric_constant,
    TokenType::r_bracket,
    TokenType::l_bracket,
    TokenType::numeric_constant,
    TokenType::r_bracket,
    TokenType::equal,
    TokenType::l_brace,
    TokenType::l_brace,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::numeric_constant,
    TokenType::r_brace,
    TokenType::comma,
    TokenType::l_brace,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::numeric_constant,
    TokenType::r_brace,
    TokenType::r_brace,
    TokenType::semicolon,
    TokenType::new_line,


    // comment
    TokenType::comment,
    TokenType::new_line,

    // int a = 5, b = 2;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // int arithmetic = a + b - a * b / b % a;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::identifier,
    TokenType::plus,
    TokenType::identifier,
    TokenType::minus,
    TokenType::identifier,
    TokenType::star,
    TokenType::identifier,
    TokenType::slash,
    TokenType::identifier,
    TokenType::percent,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // int logical = (a > b) && (b < 10) || !(a == b);
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::greater,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::double_ampersand,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::less,
    TokenType::numeric_constant,
    TokenType::r_parentheses,
    TokenType::double_pipe,
    TokenType::bang,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::equal_equal,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // int bitwise = (a & b) | (a ^ b) << 1 >> 1;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::ampersand,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::pipe,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::caret,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::l_shift,
    TokenType::numeric_constant,
    TokenType::r_shift,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    // int ternary = (a > b) ? a : b;
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::greater,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::question_mark,
    TokenType::identifier,
    TokenType::colon,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // if (a > 0) {
    TokenType::kw_if,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::greater,
    TokenType::numeric_constant,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // for (int i = 0; i < 5; ++i) {
    TokenType::kw_for,
    TokenType::l_parentheses,
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::identifier,
    TokenType::less,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::plus_plus,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // printf("i = %d\n", i);
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::string_literal,
    TokenType::comma,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // }
    TokenType::r_brace,
    TokenType::new_line,

    // } else if (a == 0) {
    TokenType::r_brace,
    TokenType::kw_else,
    TokenType::kw_if,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::equal_equal,
    TokenType::numeric_constant,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // printf("Zero\n");
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::string_literal,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // } else {
    TokenType::r_brace,
    TokenType::kw_else,
    TokenType::l_brace,
    TokenType::new_line,

    // printf("Negative\n");
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::string_literal,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // }
    TokenType::r_brace,
    TokenType::new_line,

    // switch (a) {
    TokenType::kw_switch,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // case 1:
    TokenType::kw_case,
    TokenType::numeric_constant,
    TokenType::colon,
    TokenType::new_line,

    // break;
    TokenType::kw_break,
    TokenType::semicolon,
    TokenType::new_line,

    // case 5:
    TokenType::kw_case,
    TokenType::numeric_constant,
    TokenType::colon,
    TokenType::new_line,

    // goto label_test;
    TokenType::kw_goto,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // default:
    TokenType::kw_default,
    TokenType::colon,
    TokenType::new_line,

    // break;
    TokenType::kw_break,
    TokenType::semicolon,
    TokenType::new_line,

    // }
    TokenType::r_brace,
    TokenType::new_line,

    // label_test:
    TokenType::identifier,
    TokenType::colon,
    TokenType::new_line,

    // while (b--) {
    TokenType::kw_while,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::minus_minus,
    TokenType::r_parentheses,
    TokenType::l_brace,
    TokenType::new_line,

    // continue;
    TokenType::kw_continue,
    TokenType::semicolon,
    TokenType::new_line,

    // }
    TokenType::r_brace,
    TokenType::new_line,

    // do {
    TokenType::kw_do,
    TokenType::l_brace,
    TokenType::new_line,

    // a--;
    TokenType::identifier,
    TokenType::minus_minus,
    TokenType::semicolon,
    TokenType::new_line,

    // } while (a > 0);
    TokenType::r_brace,
    TokenType::kw_while,
    TokenType::l_parentheses,
    TokenType::identifier,
    TokenType::greater,
    TokenType::numeric_constant,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // sizeof and type casting
    TokenType::identifier,
    TokenType::identifier,
    TokenType::equal,
    TokenType::kw_sizeof,
    TokenType::l_parentheses,
    TokenType::kw_struct,
    TokenType::identifier,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    TokenType::kw_double,
    TokenType::identifier,
    TokenType::equal,
    TokenType::l_parentheses,
    TokenType::kw_double,
    TokenType::r_parentheses,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // restrict pointers
    TokenType::kw_int,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::comma,
    TokenType::identifier,
    TokenType::equal,
    TokenType::numeric_constant,
    TokenType::semicolon,
    TokenType::new_line,

    TokenType::kw_int,
    TokenType::star,
    TokenType::kw_restrict,
    TokenType::identifier,
    TokenType::equal,
    TokenType::ampersand,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    TokenType::kw_int,
    TokenType::star,
    TokenType::kw_restrict,
    TokenType::identifier,
    TokenType::equal,
    TokenType::ampersand,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // *rx += *ry;
    TokenType::star,
    TokenType::identifier,
    TokenType::plus_equal,
    TokenType::star,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // printf("Done.\n");
    TokenType::identifier,
    TokenType::l_parentheses,
    TokenType::string_literal,
    TokenType::r_parentheses,
    TokenType::semicolon,
    TokenType::new_line,

    // return EXIT_SUCCESS;
    TokenType::kw_return,
    TokenType::identifier,
    TokenType::semicolon,
    TokenType::new_line,

    // }
    TokenType::r_brace,
    TokenType::new_line,

    // EOF
    TokenType::eof
};
