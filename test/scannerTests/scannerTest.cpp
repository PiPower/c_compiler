#include "../../include/frontend/scanner.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#define BUFFER_SIZE 10000
using namespace std;

string translateToken(Token token);

int main()
{
    string sourceCode = "";
    char* readBuffer = new char[BUFFER_SIZE];
    int flags = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, flags | O_NONBLOCK);
    while (true)
    {
        int size = read(0, readBuffer, BUFFER_SIZE);
        if(size == -1){ break;}

        sourceCode.append(readBuffer, size);
        if(size < BUFFER_SIZE)
        {
            break;
        }
    }

    string retrievedSourceCode = "";
    Scanner scanner(sourceCode.c_str());
    while (scanner.getCurrentToken().type != TokenType::END_OF_FILE)
    {
        if(scanner.getCurrentToken().type == TokenType::R_BRACE )
        {
            int x =2;
        }
        retrievedSourceCode+= translateToken(scanner.popToken());
    }
    cout<<retrievedSourceCode << endl;
    
}

string translateToken(Token token)
{
static int line = 0;
static const char* charTable[] = { "break", "case", "continue",
    "default", "do", "else",
    "goto", "if", "switch", "for",
    "return", "sizeof", "struct",
    "union", "while", "enum",
// type qualifier
    "restring", "const", "volatile",
// function specifier
    "inline",
// storage specifiers 
    "register", "extern", "static", "auto", "typedef",
//  keywords types
    "void", "char", "short", "int", "long", "float", 
    "double", "signed", "unsigned", "_bool", "_comples",
// separators
    "[",  "]", "(",  ")", 
    "{", "}",
// math ops  
    "&", "*", "+", "-", "~", "!", "/", 
    "%", "<", ">",  "<<",
    ">>", "<=", ">=", "==", "!=",
    "|", "^", "&&", "||", "?",
// assignemnt types
    "=", "*=", "/=", "%=", "+=", 
    "-=", "<<=", ">>=", "&=",
    "^=", "|=",
// miscallenous
     ":", "," , ";", ".", "->", "++", "--"};
    int tokenID = (int)token.type;
    string out = "";
    if(token.line > line)
    {
        out+= "\n";
        line = +token.line;
    }

    if( tokenID >= (int)TokenType::BREAK)
    {
        int tableIndex = tokenID - (int)TokenType::BREAK;
        out+= " " ;
        out+= charTable[tableIndex];
    }
    if(token.type == TokenType::CONSTANT)
    {
        out+= " " + to_string(token.context.int_32) + " ";
    }

    if(token.type == TokenType::IDENTIFIER)
    {
        out+= " " + *(string*)token.context.data + " ";
    }

    return out;
}