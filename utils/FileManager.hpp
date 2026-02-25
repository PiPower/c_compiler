#pragma once
#include <vector>
#include <inttypes.h>

struct FILE_STATE
{
    const char* path;
    size_t pathLen;
    size_t filenameOffset;
    char* fileData;
    int64_t fileSize;
};

struct FILE_ID
{
    size_t id; // index into files array
};

struct FileManager
{
    FileManager(const std::vector<const char*>& filenames,
                const std::vector<size_t>&  filenameLens);

    void AddNewPage();
    void LoadFileIntoPage(int fd, int64_t fileSize, const char* filename);
    void ManagerExitOnError(int type, const void* errorData, const char* fileName);
    void ManagerExitOnErrorCode(int errorNum, const char* fileName);
    void ManagerExitOnErrorMsg(const char* errorMsg, const char* fileName);

    std::vector<FILE_STATE> m_files;
    std::vector<char*> m_filePages;
    size_t m_currentPage;
    int64_t m_offsetIntoPage;
};
