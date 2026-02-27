#include "Parser.hpp"

Parser::Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
m_PP(mainFile, manager, opts), m_opts(opts)
{
   
}

void Parser::Parse()
{
    Token token;
    m_PP.Peek(&token);
    while (token.type != TokenTypes::eof)
    {
        /* code */
    }
    
}
