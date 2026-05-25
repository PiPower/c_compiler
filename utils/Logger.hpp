#pragma once
#include "FileManager.hpp"
#include "../frontend/TokenTypes.hpp"

struct Logger
{
    Logger(FileManager* fm, const char* modName);
    void IssueWarningImpl(
        const Token *token,
        const char *errMsg,
        ...);

    void IssueWarningImpl(
        const FILE_ID* fileId,
        const SourceLocation* loc,
        const char* errMsg,
        ...);

    void IssueWarningImpl(
        const FILE_ID* fileId,
        const SourceLocation* loc,
        const char* errMsg,
        va_list args);

    FileManager* fm;
    std::string modName;
};
