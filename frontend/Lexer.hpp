#pragma once
#include "../utils/FileManager.hpp"
#include "TokenTypes.hpp"
#include <stack>
#include "../utils/CompilationOpts.hpp"
#include <queue>
struct FilePos;
struct SourceLocation;
struct Token;
struct SizedChar;

struct Lexer
{
    Lexer(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts);
    bool IsHorizontalWhiteSpace(char C);
    bool IsVerticalWhiteSpace(char C);
    void TrigraphWarning(SourceLocation loc);
    void TrigraphWarning(const SourceLocation* loc);
    bool IsWhiteSpace(char C);
    bool IsSimpleChar(char C);
    void ChangeLexedFile();
    SourceLocation GetCurrLoc();
    char GetCurrChar();
    char GetNextChar();
    char GetCharSlow();
    char LookAhead(size_t n);
    void ConsumeChar();
    void SkipHorizontalWhiteSpace();
    int64_t ParseComment();
    int32_t Lex(Token* token);

    FILE_STATE mainFile;
    FileManager* manager;
    std::stack<FilePos> files;
    size_t currChar;
    std::vector<char> charHistory;
    std::queue<SourceLocation> currLocations;
    const char* fEnd;
    const char* fCurr;
    const CompilationOpts* opts;
};

struct SizedChar 
{
    const char* ptr;
    uint64_t len;
};


struct FilePos
{
    FILE_ID fileId;
    int64_t lineNr;
    const char* fileBase;
    const char* fileCurrent;
    const char* fileEnd;
};

struct SourceLocation
{
    SourceLocation() = default;
    SourceLocation(const FilePos& filePos, const char* fileCurr, int64_t len) :
     id(filePos.fileId),offset(fileCurr - filePos.fileBase),line(filePos.lineNr), len(len)
    {}

    FILE_ID id;
    int64_t offset;
    int64_t line;
    int64_t len;
};

struct Token
{
    TokenType::Type type;
    SourceLocation location;
};
