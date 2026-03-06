#include "Parser.hpp"
#include <cassert>
Parser::Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts)
:
PP(mainFile, manager, opts), opts(opts)
{
    assert(opts != nullptr);
}

void Parser::Parse()
{
    Token token;
    PP.Peek(&token);
    while (token.type != TokenType::eof)
    {
        PP.Peek(&token);
    }

    
    
}
