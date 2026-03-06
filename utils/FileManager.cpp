#include "FileManager.hpp"
#include <cassert>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h> 
#include <limits>

#define CPU_PAGE_SIZE 4096
#define FILEDATA_PAGE_SIZE (50 * CPU_PAGE_SIZE)
#define FILENAME_PAGE_SIZE (6 * CPU_PAGE_SIZE)

FileManager::FileManager(const std::vector<const char*>& filenames,
                         const std::vector<size_t>&  filenameLens)
    :
    fileBuffer({}, 0, 0), filenameBuffer({}, 0, 0)
{
    AddNewFilePage();
    AddNewFilenamePage();

    assert(filenameLens.size() == filenames.size());
    size_t maxLen = *std::max_element(filenameLens.begin(), filenameLens.end()) + 1;
    char* pathBuffer;
    if(maxLen < 1000'000) { pathBuffer = (char*)alloca(maxLen);}
    else { printf("Filename too long\n"); exit(-1);}
    
    for(size_t i =0; i < filenames.size(); i++)
    {
        memcpy(pathBuffer, filenames[i], filenameLens[i]);
        pathBuffer[filenameLens[i]] = '\0';

        if(TryLoadFile(pathBuffer, filenameLens[i], nullptr) < 0 )
        {
            printf("File %s does not exist", pathBuffer);
            exit(-1);
        }
    }
}

void FileManager::AddNewFilePage()
{
    void* mmapRet = mmap(nullptr, FILEDATA_PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(mmapRet == MAP_FAILED )
    {
        ManagerExitOnErrorCode(errno, nullptr);
    }
    
    char* filePage = (char*)mmapRet;
    fileBuffer.pages.push_back(filePage);
    
    fileBuffer.offsetIntoPage = 0;
    fileBuffer.currentPage = fileBuffer.pages.size() - 1;
}

void FileManager::AddNewFilenamePage()
{
    void* mmapRet = mmap(nullptr, FILENAME_PAGE_SIZE, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(mmapRet == MAP_FAILED )
    {
        ManagerExitOnErrorCode(errno, nullptr);
    }
    
    char* filePage = (char*)mmapRet;
    filenameBuffer.pages.push_back(filePage);
    
    filenameBuffer.offsetIntoPage = 0;
    filenameBuffer.currentPage = filenameBuffer.pages.size() - 1;
}

void FileManager::LoadFilenameIntoPage(const char *filename, char **pagedFilename)
{
    std::vector<char*>& filenamePages = filenameBuffer.pages;
    size_t& currentPage = filenameBuffer.currentPage;
    int64_t& offsetIntoPage =  filenameBuffer.offsetIntoPage;
    size_t filenameLen = strlen(filename);

    if(filenameLen + offsetIntoPage > FILENAME_PAGE_SIZE)
    {
        if(filenameLen > FILENAME_PAGE_SIZE ){printf("Filename too long\n"); exit(-1);}
        else{AddNewFilenamePage();}
    }

    char* page = filenamePages[currentPage];
    *pagedFilename = page + offsetIntoPage;
    memcpy(page + offsetIntoPage, filename, filenameLen);
    offsetIntoPage += filenameLen;
}

void FileManager::LoadFileIntoPage(int fd, int64_t fileSize, const char* filename, char** filePos)
{
    std::vector<char*>& filePages = fileBuffer.pages;
    size_t& currentPage = fileBuffer.currentPage;
    int64_t& offsetIntoPage =  fileBuffer.offsetIntoPage;

    if(fileSize + offsetIntoPage > FILEDATA_PAGE_SIZE)
    {
        if(fileSize > FILEDATA_PAGE_SIZE){printf("File too large\n"); exit(-1);}
        else{AddNewFilePage();}
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

int32_t FileManager::TryLoadFile(const char *filename, uint64_t nameLen, FILE_ID* loadedFile)
{
    FILE_ID  doesExist = {};
    if( GetFileId(filename, nameLen, &doesExist) == 0 )
    {
        if(loadedFile){*loadedFile = doesExist;}
        return 1;
    }

    // first load a file 
    int fd = open(filename, O_RDONLY);
    if(fd == -1 && errno == ENOENT){ return -1;}
    else if(fd == -1) {ManagerExitOnErrorCode(errno,filename);}

    off_t fileSize =  lseek(fd, 0, SEEK_END);
    if(fileSize == -1){ManagerExitOnErrorCode(errno, filename);}

    off_t startFile =  lseek(fd, 0, SEEK_SET);
    if(startFile == -1){ManagerExitOnErrorCode(errno, filename);}

    char* filePos = nullptr;
    LoadFileIntoPage(fd, fileSize, filename, &filePos);

    int ret = close(fd);
    if(ret == -1){ManagerExitOnErrorCode(errno, filename);}


    // then add descriptor of that file to file system
    char* path;
    LoadFilenameIntoPage(filename, &path);
    int64_t filenameLen = strlen(filename);
    int64_t filenameOffset = filenameLen;
    while (filenameOffset > 1 && filename[filenameOffset] != '/') { filenameOffset--;}

    fileStates.emplace_back(path, filenameLen, filenameLen - filenameOffset, filePos, fileSize);
    if(loadedFile)
    {
        loadedFile->id = fileStates.size() - 1;
    }
    return 0;
}

int32_t FileManager::GetFileState(const char *path, uint64_t pathLen, FILE_STATE *fileState)
{
    for(size_t i =0; i < fileStates.size(); i++)
    {
        if(fileStates[i].pathLen == pathLen)
        {
            if(memcmp(path, fileStates[i].path, pathLen) == 0)
            {
                *fileState = fileStates[i];
                return 0;
            }
        }
    }

    return -1;
}

int32_t FileManager::GetFileState(const FILE_ID *fileId, FILE_STATE* fileState)
{
    if(fileId->id >= fileStates.size())
    {
        return -1;
    }
    *fileState = fileStates[fileId->id];
    return 0;
}

int32_t FileManager::GetFileId(const char *path, uint64_t pathLen, FILE_ID *fileId)
{
    for(size_t i =0; i < fileStates.size(); i++)
    {
        if(fileStates[i].pathLen == pathLen)
        {
            if(memcmp(path, fileStates[i].path, pathLen) == 0)
            {

                fileId->id = i;
                return 0;
            }
        }
    }

    return -1;
}

int32_t FileManager::GetFullFilePath(const FILE_ID *fileId, std::string *path)
{
    FILE_STATE fileState;
    int32_t err = GetFileState(fileId, &fileState);
    if(err != 0) {return err;}

    return GetFullFilePath(&fileState, path);
}

int32_t FileManager::GetFullFilePath(const FILE_STATE *fileState, std::string *path)
{
    printf("GetFullFilePath function is not implemented \n");
    exit(-1);
}
