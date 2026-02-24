#pragma once
#include "utils/CompilationOpts.hpp"
#include "utils/FileManager.hpp"

struct Compiler
{
    Compiler(int argc, char* argv[]);
    void compile();

    int argc;
    char** argv;
    CompilationOpts opts;
    FileManager fileManager;
};
