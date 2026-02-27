#pragma once
#include <vector>
#include <inttypes.h>

struct CompilationOpts
{
    CompilationOpts(int argc, const char** argv);
    bool CheckBinaryFlag(const char* arg, const char* flag);
    std::vector<const char*> filenames;
    std::vector<size_t> filenameLens;
    //flags
    uint8_t trigraphs_refrenced : 1;
    uint8_t trigraphs_enabled : 1;
};



