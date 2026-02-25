#pragma once
#include "Lexer.hpp"

struct Preprocessor
{
    Preprocessor(FILE_STATE m_mainFile, FileManager* manager);

    Lexer m_lexer;
};
