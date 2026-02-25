#pragma once
#include <vector>

struct CompilationOpts
{
    CompilationOpts(int argc, const char** argv);

    std::vector<const char*> m_filenames;
    std::vector<size_t> m_filenameLens;
};



