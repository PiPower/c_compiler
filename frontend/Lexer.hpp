#pragma once
#include "../utils/FileManager.hpp"
#include "TokenTypes.hpp"
#include <stack>

struct FilePos;
struct SourceLocation;
struct Token;
struct SizedChar;

struct Lexer
{
    Lexer(FILE_STATE mainFile, FileManager* manager);
    bool IsHorizontalWhiteSpace(char C);
    char GetNextChar();
    int32_t Lex(Token* token);

    FILE_STATE m_mainFile;
    FileManager* m_manager;
    std::stack<FilePos> m_files;
    const char* m_fEnd;
    const char* m_fCurr;
};

struct SizedChar 
{
    const char* ptr;
    uint64_t len;
};


struct FilePos
{
    FILE_ID fileId;
    const char* fileBase;
    const char* fileEnd;
    const char* fileCurr;
};

struct SourceLocation
{
    FILE_ID id;
    int64_t offset;
    int64_t line;
};

struct Token
{
    TokenTypes::Type type;
    SourceLocation location;
};
