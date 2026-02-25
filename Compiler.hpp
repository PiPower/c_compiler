#pragma once
#include "utils/CompilationOpts.hpp"
#include "utils/FileManager.hpp"

struct Compiler
{
    Compiler(int argc, char* argv[]);
    void compile();

    int m_argc;
    char** m_argv;
    CompilationOpts m_opts;
    FileManager m_fileManager;
};
