#include "Preprocessor.hpp"



Preprocessor::Preprocessor(FILE_STATE m_mainFile, FileManager *manager, const CompilationOpts* opts)
:
m_lexer(m_mainFile, manager, opts), m_opts(opts)
{
}

int32_t Preprocessor::Peek(Token* token)
{
    int32_t ret = m_lexer.Lex(token);
    return ret;
}
