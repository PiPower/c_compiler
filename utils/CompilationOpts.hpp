#pragma once
#include <vector>
#include <inttypes.h>

struct CompilationOpts
{
    CompilationOpts(int argc, const char** argv);

    std::vector<const char*> m_filenames;
    std::vector<size_t> m_filenameLens;
    //flags
    uint8_t trigraphs_set : 1;
    uint8_t trigraphs_enabled : 1;
};



