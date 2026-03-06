#pragma once
#include <vector>
#include <inttypes.h>
#include <string>
struct FILE_STATE
{

    const char* path; // pointer to file path
    size_t pathLen; // number of chars in path
    size_t filenameOffset; // offset of first character in path that represents file name
    const char* fileData; // pointer to file data loaded in filePages
    int64_t fileSize; // size of file
};

struct FILE_ID
{
    size_t id; // index into files array
};

struct PagedBuffer
{
    std::vector<char*> pages;
    size_t currentPage;
    int64_t offsetIntoPage;
};


/*
    Files are stored in filePages and filenames in filenamePages
*/
struct FileManager
{
    FileManager(const std::vector<const char*>& filenames,
                const std::vector<size_t>&  filenameLens);

    int32_t TryLoadFile(const char *filename, uint64_t nameLen, FILE_ID* loadedFile);
    int32_t GetFileState(const char* path, uint64_t pathLen, FILE_STATE* fileState);
    int32_t GetFileState(const FILE_ID* fileId, FILE_STATE* fileState);
    int32_t GetFileId(const char* path, uint64_t pathLen, FILE_ID* fileId);
    int32_t GetFullFilePath(const FILE_ID* fileId, std::string* path);
    int32_t GetFullFilePath(const FILE_STATE* fileState, std::string* path);

private:
    void AddNewFilePage();
    void AddNewFilenamePage();
    void LoadFilenameIntoPage(const char* filename, char** pagedFilename);
    void LoadFileIntoPage(int fd, int64_t fileSize, const char* filename, char** filePos);
    void ManagerExitOnError(int type, const void* errorData, const char* fileName);
    void ManagerExitOnErrorCode(int errorNum, const char* fileName);
    void ManagerExitOnErrorMsg(const char* errorMsg, const char* fileName);

public:
    std::vector<FILE_STATE> fileStates;
    PagedBuffer fileBuffer;
    PagedBuffer filenameBuffer;

};
