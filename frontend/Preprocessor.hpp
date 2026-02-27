#pragma once
#include "Lexer.hpp"

struct Preprocessor
{
    Preprocessor(FILE_STATE m_mainFile, FileManager* manager, const CompilationOpts* opts);
    int32_t Peek(Token* token);

    Lexer m_lexer;
    const CompilationOpts* m_opts;
};
