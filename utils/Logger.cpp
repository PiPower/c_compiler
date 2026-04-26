#include "Logger.hpp"
#include <stdarg.h>
#include <string.h>
Logger::Logger(FileManager *fm)
:
fm(fm)
{
}

void Logger::IssueWarningImpl(
    const char *moduleName,
    const Token *token,
    const char *errMsg,
    ...)
{
    va_list args;
    va_start(args, errMsg);
    if(token) {IssueWarningImpl(moduleName, &token->location.id, &token->location, errMsg, args);}
    else{IssueWarningImpl(moduleName, nullptr, nullptr, errMsg, args);}
    va_end(args);

    return;
}

void Logger::IssueWarningImpl(
        const char* moduleName,
        const FILE_ID* fileId,
        const SourceLocation* loc,
        const char* errMsg,
        va_list args)
{
    if(fileId)
    {
        FILE_STATE fileState;
        fm->GetFileState(fileId, &fileState);
        char* pathBuffer = (char*)alloca(fileState.pathLen + 1);
        memcpy(pathBuffer, fileState.path, fileState.pathLen);
        pathBuffer[fileState.pathLen] = '\0';
        printf("%s:", pathBuffer);
    }

    if(loc)
    {
        printf("%ld:%ld", loc->line, loc->offset);
    }

    printf("%s warning: \n");

    if(errMsg)
    {
        vprintf(errMsg, args);
    }
    printf("\n");
}
