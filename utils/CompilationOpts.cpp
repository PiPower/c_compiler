#include "CompilationOpts.hpp"
#include <stdio.h>
static bool isCFilePath(const char* arg, size_t* len)
{
    size_t j = 0;
    while (arg[j] != '\0'){j++;}
    j--;
    while (arg[j] == ' ') {j--;}

    if(len)
    {
        *len = j + 1;
    }
    if(j > 2 && arg[j] == 'c' && arg[j - 1] == '.' && 
       (( 'A' <= arg[j - 2 ] && arg[j - 2 ] <= 'Z') || 
        ( 'a' <= arg[j - 2 ] && arg[j - 2 ] <= 'z' ) ||
        ( '1' <= arg[j - 2 ] && arg[j - 2 ] <= '9' )))
    {
        return true;
    }

    return false;
}

CompilationOpts::CompilationOpts(int argc, const char** argv)
:
longestPath(0)
{
    trigraphs_refrenced = 0;
    trigraphs_enabled = 1;

    for(int i = 1; i < argc; i++)
    {
        const char* arg = argv[i];
        size_t fileLen = 0;
        if(isCFilePath(arg, &fileLen))
        {
            filenames.push_back(arg);
            filenameLens.push_back(fileLen);
        }
        else if(CheckBinaryFlag(arg, "-ftrigraphs"))
        {
            trigraphs_refrenced = 1;
            trigraphs_enabled = 1;
        }
        else if(CheckBinaryFlag(arg, "-fno-trigraphs"))
        {
            trigraphs_refrenced = 1;
            trigraphs_enabled = 0;
        }
        else if(arg[0] != '\0' && arg[0] == '-' && 
                arg[1] != '\0' && arg[1] == 'I')
        {
            size_t len = 0;
            size_t i = 2;
            while (arg[i] != ' ' && arg[i] != '\0')
            {
                len++; 
                i++;
            }
            AddSearchPath(arg + 2, len);
        }
        else
        {
            printf("Uknown argument\nPos: %d\nValue: %s\n", i - 1, arg);
            exit(-1);
        }

    }
}

bool CompilationOpts::CheckBinaryFlag(const char *arg, const char *flag)
{
    while (*arg != '\0' && *flag != '\0')
    {
        if(*arg != *flag) {return false;}
        arg++;
        flag++;
    }
    
    return *arg == *flag;
}

void CompilationOpts::AddSearchPath(const char *path, size_t len)
{
    searchPaths.emplace_back(path, len);
    longestPath = std::max(len, longestPath);
}
