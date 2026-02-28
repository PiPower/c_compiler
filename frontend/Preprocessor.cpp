#include "Preprocessor.hpp"
#include <cassert>

Preprocessor::Preprocessor(FILE_STATE mainFile, FileManager *manager, const CompilationOpts* opts)
:
lexer(mainFile, manager, opts), opts(opts)
{
    assert(opts != nullptr);
}

int32_t Preprocessor::Peek(Token* token)
{
    int32_t ret = lexer.Lex(token);
    ret = lexer.Lex(token);
    return ret;
}
