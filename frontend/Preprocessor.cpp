#include "Preprocessor.hpp"



Preprocessor::Preprocessor(FILE_STATE mainFile, FileManager *manager, const CompilationOpts* opts)
:
lexer(mainFile, manager, opts), opts(opts)
{
}

int32_t Preprocessor::Peek(Token* token)
{
    int32_t ret = lexer.Lex(token);
    return ret;
}
