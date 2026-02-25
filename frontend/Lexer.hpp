#pragma once
#include "../utils/FileManager.hpp"

struct FilePos
{
    const char* file;
    int64_t offset;
    int64_t fileLen;
};


struct Lexer
{
    Lexer(FILE_STATE mainFile, FileManager* manager);

    FILE_STATE m_currFile;
    FilePos m_currPos;
    FileManager* m_manager;
};
