#include "Logger.hpp"
#include <stdarg.h>
#include <string.h>
Logger::Logger(FileManager *fm, const char* modName)
:
fm(fm), modName(modName)
{
}

void Logger::IssueWarningImpl(
    const Token *token,
    const char *errMsg,
    ...)
{
    va_list args;
    va_start(args, errMsg);
    if(token) {IssueWarningImpl(&token->location.id, &token->location, errMsg, args);}
    else{IssueWarningImpl(nullptr, nullptr, errMsg, args);}
    va_end(args);

    return;
}

void Logger::IssueWarningImpl(
    const FILE_ID *fileId, 
    const SourceLocation *loc, 
    const char *errMsg, 
    ...)
{
    va_list args;
    va_start(args, errMsg);
    IssueWarningImpl(fileId, loc, errMsg, args);
    va_end(args);

    return;
}

void Logger::IssueWarningImpl(
        const FILE_ID* fileId,
        const SourceLocation* loc,
        const char* errMsg,
        va_list args)
{
    if(fileId)
    {
        FILE_STATE fileState;
        if(fm->GetFileState(fileId, &fileState) == 0)
        {
            char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
            memcpy(pathBuffer, fileState.path, fileState.pathLen);
            pathBuffer[fileState.pathLen] = '\0';
            printf("%s:", pathBuffer);
        }
        else
        {
            printf("<unknown-file>\n");
            exit(-1);
        }
    }

    if(loc)
    {
        printf("%ld:%ld\n", loc->line, loc->offset);
    }

    printf("%s warning: \n", modName.c_str());

    if(errMsg)
    {
        vprintf(errMsg, args);
    }
    printf("\n");
}
