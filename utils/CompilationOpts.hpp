#pragma once
#include <vector>

struct CompilationOpts
{
    CompilationOpts(int argc, const char** argv);

    std::vector<const char*> filenames;
    std::vector<size_t> filenameLens;
};



