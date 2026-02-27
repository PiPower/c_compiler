#include "Parser.hpp"

Parser::Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
PP(mainFile, manager, opts), opts(opts)
{
   
}

void Parser::Parse()
{
    Token token;
    PP.Peek(&token);
    while (token.type != TokenTypes::eof)
    {
        /* code */
    }
    
}
