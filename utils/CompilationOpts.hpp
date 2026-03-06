#pragma once
#include <vector>
#include <inttypes.h>
#include <string>

struct CompilationOpts
{
    CompilationOpts(int argc, const char** argv);
    bool CheckBinaryFlag(const char* arg, const char* flag); 
    void AddSearchPath(const char* path, size_t len);

    std::vector<const char*> filenames;
    std::vector<size_t> filenameLens;
    std::vector<std::string> searchPaths;
    size_t longestPath;
    //flags
    uint8_t trigraphs_refrenced : 1;
    uint8_t trigraphs_enabled : 1;
};



