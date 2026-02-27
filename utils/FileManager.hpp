#pragma once
#include <vector>
#include <inttypes.h>

struct FILE_STATE
{
    const char* path; // pointer to file path
    size_t pathLen; // length of file path in path
    size_t filenameOffset; // offset of first character in path that represents file name
    char* fileData; // pointer to file data loaded in m_filePages
    int64_t fileSize; // size of file
};

struct FILE_ID
{
    size_t id; // index into files array
};

/*
    Files are stored in filePages
*/
struct FileManager
{
    FileManager(const std::vector<const char*>& filenames,
                const std::vector<size_t>&  filenameLens);

    void AddNewPage();
    void LoadFileIntoPage(int fd, int64_t fileSize, const char* filename, char** filePos);
    void ManagerExitOnError(int type, const void* errorData, const char* fileName);
    void ManagerExitOnErrorCode(int errorNum, const char* fileName);
    void ManagerExitOnErrorMsg(const char* errorMsg, const char* fileName);
    int32_t GetFileState(const char* path, uint64_t pathLen, FILE_STATE* fileState);
    int32_t GetFileId(const char* path, uint64_t pathLen, FILE_ID* fileId);

    std::vector<FILE_STATE> m_files;
    std::vector<char*> m_filePages;
    size_t m_currentPage;
    int64_t m_offsetIntoPage;
};
