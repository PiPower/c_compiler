#include <vector>
#include <cstddef>
#include <iostream>
#include "../frontend/Lexer.hpp"
extern std::vector<TokenType::Type> tokens;

int main()
{
    const char* path = "examples/testfile.c";
    const char* arr[] = {path};
    FileManager fileManager({path}, {23});
    FILE_STATE main;
    fileManager.GetFileState("examples/testfile.c", 23,&main);
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


std::vector<TokenType::Type> tokens = {

// #include <stdio.h>
TokenType::hash, TokenType::pp_include, TokenType::less,
TokenType::identifier, TokenType::dot, TokenType::identifier,
TokenType::greater, TokenType::new_line,

// #define MAX 100
TokenType::hash, TokenType::pp_define, TokenType::identifier,
TokenType::numeric_constant, TokenType::new_line,

TokenType::new_line,

// typedef struct Point { int x, y; } Point_t;
TokenType::kw_typedef, TokenType::kw_struct, TokenType::identifier,
TokenType::l_brace,
TokenType::kw_int, TokenType::identifier, TokenType::comma,
TokenType::identifier, TokenType::semicolon,
TokenType::r_brace, TokenType::identifier, TokenType::semicolon,
TokenType::new_line,

TokenType::new_line,

// int main(void) {
TokenType::kw_int, TokenType::identifier,
TokenType::l_parentheses, TokenType::kw_void,
TokenType::r_parentheses,
TokenType::l_brace,
TokenType::new_line,

// int \u9323 = 939, \U85483922 = 395995l;
TokenType::kw_int, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::comma, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

// char \u93F3\UAAFFf293sdsa3 = '3';
TokenType::kw_char, TokenType::identifier,
TokenType::equal, TokenType::character_literal,
TokenType::semicolon,
TokenType::new_line,

// int a = 5, b = 10;
TokenType::kw_int, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::comma, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

// float f = 3.14, g = 2.71;
TokenType::kw_float, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::comma, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

// double d = 1e3;
TokenType::kw_double, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

// long l = 1234567890L;
TokenType::kw_long, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

// char c = 'Z';
TokenType::kw_char, TokenType::identifier,
TokenType::equal, TokenType::character_literal,
TokenType::semicolon,
TokenType::new_line,

// _Bool flag = 1;
TokenType::kw_bool, TokenType::identifier,
TokenType::equal, TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

// const int ci = 42;
TokenType::kw_const, TokenType::kw_int,
TokenType::identifier, TokenType::equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::new_line,

// volatile int vi = 0;
TokenType::kw_volatile, TokenType::kw_int,
TokenType::identifier, TokenType::equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::new_line,

// if (a < b && b > 0 || !flag) {
TokenType::kw_if, TokenType::l_parentheses,
TokenType::identifier, TokenType::less,
TokenType::identifier, TokenType::double_ampersand,
TokenType::identifier, TokenType::greater,
TokenType::numeric_constant, TokenType::double_pipe,
TokenType::bang, TokenType::identifier,
TokenType::r_parentheses,
TokenType::l_brace,
TokenType::new_line,

// a += 1; b -= 1; c++; d--;
TokenType::identifier, TokenType::plus_equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::identifier, TokenType::minus_equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::identifier, TokenType::plus_plus,
TokenType::semicolon,
TokenType::identifier, TokenType::minus_minus,
TokenType::semicolon,
TokenType::new_line,

TokenType::r_brace,
TokenType::new_line,

// else {
TokenType::kw_else, TokenType::l_brace,
TokenType::new_line,

// a *= 2; b /= 2; f *= g; g /= f;
TokenType::identifier, TokenType::star_equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::identifier, TokenType::slash_equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::identifier, TokenType::star_equal,
TokenType::identifier, TokenType::semicolon,
TokenType::identifier, TokenType::slash_equal,
TokenType::identifier, TokenType::semicolon,
TokenType::new_line,

TokenType::r_brace,
TokenType::new_line,

// switch(a) {
TokenType::kw_switch, TokenType::l_parentheses,
TokenType::identifier, TokenType::r_parentheses,
TokenType::l_brace,
TokenType::new_line,

// case 1: break;
TokenType::kw_case, TokenType::numeric_constant,
TokenType::colon,
TokenType::kw_break, TokenType::semicolon,
TokenType::new_line,

// default: ;
TokenType::kw_default, TokenType::colon,
TokenType::semicolon,
TokenType::new_line,

TokenType::r_brace,
TokenType::new_line,

// while(a < MAX) {
TokenType::kw_while, TokenType::l_parentheses,
TokenType::identifier, TokenType::less,
TokenType::identifier,
TokenType::r_parentheses,
TokenType::l_brace,
TokenType::new_line,

// a <<= 1;
TokenType::identifier, TokenType::l_shift_equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::new_line,

// b >>= 1;
TokenType::identifier, TokenType::r_shift_equal,
TokenType::numeric_constant, TokenType::semicolon,
TokenType::new_line,

TokenType::r_brace,
TokenType::new_line,

// Point_t p = { .x = a, .y = b };
TokenType::identifier, TokenType::identifier,
TokenType::equal,
TokenType::l_brace,
TokenType::dot, TokenType::identifier,
TokenType::equal, TokenType::identifier,
TokenType::comma,
TokenType::dot, TokenType::identifier,
TokenType::equal, TokenType::identifier,
TokenType::r_brace,
TokenType::semicolon,
TokenType::new_line,

// Point_t* ptr = &p;
TokenType::identifier, TokenType::star,
TokenType::identifier, TokenType::equal,
TokenType::ampersand, TokenType::identifier,
TokenType::semicolon,
TokenType::new_line,

// ptr->x = 0;
TokenType::identifier, TokenType::arrow,
TokenType::identifier, TokenType::equal,
TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

// comment line
TokenType::comment,
TokenType::new_line,

// return 0;
TokenType::kw_return,
TokenType::numeric_constant,
TokenType::semicolon,
TokenType::new_line,

TokenType::r_brace,

TokenType::eof
};
