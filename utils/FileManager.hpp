#pragma once
#include <vector>
struct FILE_STATE
{
    int fd;
    const char* path;
    size_t pathLen;
    size_t filenameOffset;
};

struct FileManager
{
    FileManager(const std::vector<const char*>& filenames,
                const std::vector<size_t>&  filenameLens);

    std::vector<FILE_STATE> files;
};
