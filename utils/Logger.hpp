#include "FileManager.hpp"
#include "../frontend/TokenTypes.hpp"

struct Logger
{
    Logger(FileManager* fm);
    void IssueWarningImpl(
        const char* moduleName,
        const Token *token,
        const char *errMsg,
        ...);

    void IssueWarningImpl(
        const char* moduleName,
        const FILE_ID* fileId,
        const SourceLocation* loc,
        const char* errMsg,
        va_list args);

    FileManager* fm;
};
