#include "FileManager.hpp"
#include <cassert>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h> 

#define CPU_PAGE_SIZE 4096
#define PAGE_COUNT 50
#define PAGE_SIZE (PAGE_COUNT * CPU_PAGE_SIZE)

FileManager::FileManager(const std::vector<const char*>& filenames,
                         const std::vector<size_t>&  filenameLens)
{
    AddNewPage();
    assert(filenameLens.size() == filenames.size());
    size_t maxLen = *std::max_element(filenameLens.begin(), filenameLens.end());
    char* pathBuffer;
    if(maxLen < 1000'000) { pathBuffer = (char*)alloca(maxLen);}
    else { pathBuffer = new char[maxLen];}
    
    for(size_t i =0; i < filenames.size(); i++)
    {
        size_t nameLen = filenameLens[i]; 
        while (nameLen > 0 && filenames[i][nameLen] != '/') { nameLen--;}
        
        files.push_back({});
        FILE_STATE& state = files.back();
        state.path = filenames[i];
        state.pathLen = filenameLens[i];
        state.filenameOffset = nameLen + 1;
        memcpy(pathBuffer, state.path, state.pathLen);
        pathBuffer[state.pathLen] = '\0';

        int fd = open(pathBuffer, O_RDONLY);
        if(fd == -1){ManagerExitOnErrorCode(errno, pathBuffer);}

        off_t fileSize =  lseek(fd, 0, SEEK_END);
        if(fileSize == -1){ManagerExitOnErrorCode(errno, pathBuffer);}
        state.fileSize = static_cast<int64_t>(fileSize);

        off_t startFile =  lseek(fd, 0, SEEK_SET);
        if(startFile == -1){ManagerExitOnErrorCode(errno, pathBuffer);}

        LoadFileIntoPage(fd, state.fileSize, pathBuffer, &state.fileData);

        int ret = close(fd);
        if(ret == -1){ManagerExitOnErrorCode(errno, pathBuffer);}
    }

    if(maxLen >= 1000'000) {delete[] pathBuffer;}
}

void FileManager::AddNewPage()
{
    void* mmapRet = mmap(nullptr, PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(mmapRet == MAP_FAILED )
    {
        ManagerExitOnErrorCode(errno, nullptr);
    }
    
    char* filePage = (char*)mmapRet;
    filePages.push_back(filePage);
    
    offsetIntoPage = 0;
    currentPage = filePages.size() - 1;
}

void FileManager::LoadFileIntoPage(int fd, int64_t fileSize, const char* filename, char** filePos)
{
    if(fileSize + offsetIntoPage > PAGE_SIZE)
    {
        if(fileSize > PAGE_SIZE){exit(-1);}
        else{AddNewPage();}
    }
    char* page = filePages[currentPage];
    *filePos = page + offsetIntoPage;
    ssize_t readBytes =  read(fd, page + offsetIntoPage, fileSize);
    if(readBytes == -1){ManagerExitOnErrorCode(errno, filename);}
    if(readBytes != fileSize ){ManagerExitOnErrorMsg("Could not load whole file", filename);} 
    offsetIntoPage += fileSize;
}

void FileManager::ManagerExitOnError(int type, const void *errorData, const char *fileName)
{
    if(fileName)
    {
        printf("File Manager detected error for file: %s \n", fileName);
    }
    else
    {
        printf("File Manager detected error\n");
    }

    if(type == 0)
    {
        printf("Error Message: %s \n", strerror(*(int*)errorData));
    }
    else if(type == 1)
    {
        printf("Error Message: %s \n", (const char*)errorData);
    }
    exit(-1);

}

void FileManager::ManagerExitOnErrorCode(int errorNum, const char *fileName)
{
    ManagerExitOnError(0, &errorNum, fileName);
}

void FileManager::ManagerExitOnErrorMsg(const char *errorMsg, const char *fileName)
{
    ManagerExitOnError(1, errorMsg, fileName);
}

int32_t FileManager::GetFileState(const char *path, uint64_t pathLen, FILE_STATE* fileState)
{
    for(size_t i =0; i < files.size(); i++)
    {
        if(files[i].pathLen == pathLen)
        {
            if(memcmp(path, files[i].path, pathLen) == 0)
            {
                *fileState = files[i];
                return 0;
            }
        }
    }

    return -1;
}

int32_t FileManager::GetFileId(const char *path, uint64_t pathLen, FILE_ID *fileId)
{
        for(size_t i =0; i < files.size(); i++)
    {
        if(files[i].pathLen == pathLen)
        {
            if(memcmp(path, files[i].path, pathLen) == 0)
            {
                fileId->id = i;
                return 0;
            }
        }
    }

    return -1;
}
