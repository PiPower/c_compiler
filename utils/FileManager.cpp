#include "FileManager.hpp"
#include <cassert>
#include <algorithm>
#include <fcntl.h>
#include <string.h>

static void ManagerExitOnError(int errorNum, const char* fileName)
{
    printf("File Magaer detected error for file: %s \n", fileName);
    printf("Error Message: %s \n", strerror(errorNum));
    exit(-1);
}


FileManager::FileManager(const std::vector<const char*>& filenames,
                         const std::vector<size_t>&  filenameLens)
{
    assert(filenameLens.size() == filenames.size());
    size_t maxLen = *std::max_element(filenameLens.begin(), filenameLens.end());
    char* pathBuffer = new char[maxLen + 1];
    
    for(size_t i =0; i < filenames.size(); i++)
    {
        size_t nameLen = filenameLens[i]; 
        while (nameLen > 0 && filenames[i][nameLen] != '/') { nameLen--;}
        
        FILE_STATE state = {};
        state.path = filenames[i];
        state.pathLen = filenameLens[i];
        state.filenameOffset = nameLen + 1;

        memcpy(pathBuffer, state.path, state.pathLen);
        pathBuffer[state.pathLen] = '\0';
        state.fd = open(pathBuffer, O_RDONLY);
        if(state.fd == -1)
        {
            ManagerExitOnError(errno, pathBuffer);
        }
        files.push_back(state);
    }

    delete[] pathBuffer;
}
