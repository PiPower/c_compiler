#include "Parser.hpp"

Parser::Parser(FILE_STATE mainFile, FileManager* manager)
:
m_PP(mainFile, manager)
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
