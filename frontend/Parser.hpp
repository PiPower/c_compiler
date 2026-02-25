#pragma once
#include "AstNode.hpp"
#include "../utils/FileManager.hpp"
#include "Preprocessor.hpp"

struct Parser
{
    Parser(FILE_STATE mainFile, FileManager* manager);
    void Parse();

    Preprocessor m_PP;
};
