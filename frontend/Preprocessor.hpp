#pragma once
#include "Lexer.hpp"

struct Preprocessor
{
    Preprocessor(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts);
    int32_t Peek(Token* token);

    Lexer lexer;
    const CompilationOpts* opts;
};
