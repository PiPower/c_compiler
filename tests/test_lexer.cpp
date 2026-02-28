#include <vector>
#include <cstddef>
#include "../frontend/Lexer.hpp"
extern std::vector<TokenType::Type> tokens;
extern std::vector<SourceLocation> locations;

int main()
{
    const char* path = "examples/tesfile.c";
    const char* arr[] = {path};
    FileManager fileManager({path}, {23});
    FILE_STATE main;
    fileManager.GetFileState("examples/tesfile.c", 23,&main);
    CompilationOpts opts(1, (const char**)arr);
    Lexer lexer(main, &fileManager, &opts);

    Token tok;
    size_t i = 0;
    do{
        lexer.Lex(&tok);
        TokenType::Type expectedToken = tokens[i];
        SourceLocation expectedLoc = locations[i];
        i++;

    }while (tok.type == TokenType::eof);

    
}


std::vector<TokenType::Type> tokens = {

// #include <stdio.h>
TokenType::hash, TokenType::identifier, TokenType::less,
TokenType::identifier, TokenType::dot, TokenType::identifier,
TokenType::greater,

// #define MAX 100
TokenType::hash, TokenType::identifier, TokenType::identifier,
TokenType::numeric_literal,

// typedef struct Point { int x, y; } Point_t;
TokenType::kw_typedef, TokenType::kw_struct, TokenType::identifier,
TokenType::l_brace,
TokenType::kw_int, TokenType::identifier, TokenType::comma,
TokenType::identifier, TokenType::semicolon,
TokenType::r_brace, TokenType::identifier, TokenType::semicolon,

// int main(void) {
TokenType::kw_int, TokenType::identifier,
TokenType::l_parentheses, TokenType::kw_void,
TokenType::r_parentheses,
TokenType::l_brace,

// int a = 5, b = 10;
TokenType::kw_int, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::comma, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::semicolon,

// float f = 3.14, g = 2.71;
TokenType::kw_float, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::comma, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::semicolon,

// double d = 1e3;
TokenType::kw_double, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::semicolon,

// long l = 1234567890L;
TokenType::kw_long, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::semicolon,

// char c = 'Z';
TokenType::kw_char, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::semicolon,

// _Bool flag = 1;
TokenType::kw_bool, TokenType::identifier,
TokenType::equal, TokenType::numeric_literal,
TokenType::semicolon,

// const int ci = 42;
TokenType::kw_const, TokenType::kw_int,
TokenType::identifier, TokenType::equal,
TokenType::numeric_literal, TokenType::semicolon,

// volatile int vi = 0;
TokenType::kw_volatile, TokenType::kw_int,
TokenType::identifier, TokenType::equal,
TokenType::numeric_literal, TokenType::semicolon,

// if (a < b && b > 0 || !flag)
TokenType::kw_if, TokenType::l_parentheses,
TokenType::identifier, TokenType::less,
TokenType::identifier, TokenType::double_ampersand,
TokenType::identifier, TokenType::greater,
TokenType::numeric_literal, TokenType::double_pipe,
TokenType::bang, TokenType::identifier,
TokenType::r_parentheses,
TokenType::l_brace,

// a += 1; b -= 1; c++; d--;
TokenType::identifier, TokenType::plus_equal,
TokenType::numeric_literal, TokenType::semicolon,
TokenType::identifier, TokenType::minus_equal,
TokenType::numeric_literal, TokenType::semicolon,
TokenType::identifier, TokenType::plus_plus,
TokenType::semicolon,
TokenType::identifier, TokenType::minus_minus,
TokenType::semicolon,

TokenType::r_brace,

// else {
TokenType::kw_else, TokenType::l_brace,

// a *= 2; b /= 2; f *= g; g /= f;
TokenType::identifier, TokenType::star_equal,
TokenType::numeric_literal, TokenType::semicolon,
TokenType::identifier, TokenType::slash_equal,
TokenType::numeric_literal, TokenType::semicolon,
TokenType::identifier, TokenType::star_equal,
TokenType::identifier, TokenType::semicolon,
TokenType::identifier, TokenType::slash_equal,
TokenType::identifier, TokenType::semicolon,

TokenType::r_brace,

// switch(a) {
TokenType::kw_switch, TokenType::l_parentheses,
TokenType::identifier, TokenType::r_parentheses,
TokenType::l_brace,

// case 1: break;
TokenType::kw_case, TokenType::numeric_literal,
TokenType::colon,
TokenType::kw_break, TokenType::semicolon,

// default:
TokenType::kw_default, TokenType::colon,
TokenType::semicolon,

TokenType::r_brace,

// while(a < MAX)
TokenType::kw_while, TokenType::l_parentheses,
TokenType::identifier, TokenType::less,
TokenType::identifier,
TokenType::r_parentheses,
TokenType::l_brace,

// a <<= 1;
TokenType::identifier, TokenType::l_shift_equal,
TokenType::numeric_literal, TokenType::semicolon,

// b >>= 1;
TokenType::identifier, TokenType::r_shift_equal,
TokenType::numeric_literal, TokenType::semicolon,

TokenType::r_brace,

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

// Point_t* ptr = &p;
TokenType::identifier, TokenType::star,
TokenType::identifier, TokenType::equal,
TokenType::ampersand, TokenType::identifier,
TokenType::semicolon,

// ptr->x = 0;
TokenType::identifier, TokenType::arrow,
TokenType::identifier, TokenType::equal,
TokenType::numeric_literal,
TokenType::semicolon,

// return 0;
TokenType::kw_return,
TokenType::numeric_literal,
TokenType::semicolon,

TokenType::r_brace,

TokenType::eof
};

std::vector<SourceLocation> locations = {
{0,0,1,1},{0,1,1,1},{0,2,1,1},{0,3,1,1},{0,4,1,1},{0,5,1,1},{0,6,1,1},
{0,7,1,1},{0,8,1,1},{0,9,1,1},{0,10,1,1},
{0,11,1,1},{0,12,1,1},{0,13,1,1},{0,14,1,1},{0,15,1,1},
{0,16,1,1},{0,17,1,1},{0,18,1,1},{0,19,1,1},{0,20,1,1},
{0,21,1,1},{0,22,1,1},{0,23,1,1},{0,24,1,1},{0,25,1,1},
{0,26,1,1},{0,27,1,1},{0,28,1,1},{0,29,1,1},{0,30,1,1},
{0,31,1,1},{0,32,1,1},{0,33,1,1},{0,34,1,1},{0,35,1,1},
{0,36,1,1},{0,37,1,1},{0,38,1,1},{0,39,1,1},{0,40,1,1},
{0,41,1,1},{0,42,1,1},{0,43,1,1},{0,44,1,1},{0,45,1,1},
{0,46,1,1},{0,47,1,1},{0,48,1,1},{0,49,1,1},{0,50,1,1},
{0,51,1,1},{0,52,1,1},{0,53,1,1},{0,54,1,1},{0,55,1,1},
{0,56,1,1},{0,57,1,1},{0,58,1,1},{0,59,1,1},{0,60,1,1},
{0,61,1,1},{0,62,1,1},{0,63,1,1},{0,64,1,1},{0,65,1,1},
{0,66,1,1},{0,67,1,1},{0,68,1,1},{0,69,1,1},{0,70,1,1},
{0,71,1,1},{0,72,1,1},{0,73,1,1},{0,74,1,1},{0,75,1,1},
{0,76,1,1},{0,77,1,1},{0,78,1,1},{0,79,1,1},{0,80,1,1},
{0,81,1,1},{0,82,1,1},{0,83,1,1},{0,84,1,1},{0,85,1,1},
{0,86,1,1},{0,87,1,1},{0,88,1,1},{0,89,1,1},{0,90,1,1},
{0,91,1,1},{0,92,1,1},{0,93,1,1},{0,94,1,1},{0,95,1,1},
{0,96,1,1},{0,97,1,1},{0,98,1,1},{0,99,1,1},{0,100,1,1},
{0,101,1,1},{0,102,1,1},{0,103,1,1},{0,104,1,1},{0,105,1,1},
{0,106,1,1},{0,107,1,1},{0,108,1,1},{0,109,1,1},{0,110,1,1},
{0,111,1,1},{0,112,1,1},{0,113,1,1},{0,114,1,1},{0,115,1,1},
{0,116,1,1},{0,117,1,1},{0,118,1,1},{0,119,1,1},{0,120,1,1},
{0,121,1,1},{0,122,1,1},{0,123,1,1},{0,124,1,1},{0,125,1,1},
{0,126,1,1},{0,127,1,1},{0,128,1,1},{0,129,1,1},{0,130,1,1},
{0,131,1,1},{0,132,1,1},{0,133,1,1},{0,134,1,1},{0,135,1,1},
{0,136,1,1},{0,137,1,1},{0,138,1,1},{0,139,1,1},{0,140,1,1},
{0,141,1,1},{0,142,1,1},{0,143,1,1},{0,144,1,1},{0,145,1,1},
{0,146,1,1},{0,147,1,1},{0,148,1,1},{0,149,1,1},{0,150,1,1},
{0,151,1,1},{0,152,1,1},{0,153,1,1},{0,154,1,1},{0,155,1,1},
{0,156,1,1},{0,157,1,0}
};