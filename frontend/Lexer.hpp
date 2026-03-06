#pragma once
#include "../utils/FileManager.hpp"
#include "TokenTypes.hpp"
#include <stack>
#include "../utils/CompilationOpts.hpp"
#include <deque>
#include <unordered_map>
#include <string_view>
#include <queue>

struct FilePos;
struct SourceLocation;
struct Token;
struct DecimalType;

struct Lexer
{
    Lexer(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts);
    int32_t Lex(Token* token);
    int32_t PushFile(FILE_ID id);
private:
    void PrepareKeywordMap();
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
    bool SkipHorizontalWhiteSpace();
    void RestoreLexerPointer();
    void IssueWarning(const char* msg, const SourceLocation* loc);
    void LexConstant(Token* token, const SourceLocation* firstNum);
    void LexIdentifier(Token* token, const SourceLocation* firstChar);
    void LexCharSequence(Token* token, const char separator);
    bool LexEscapeSequence();
    int64_t LexComment();
    int64_t LexMultilineComment();
    DecimalType CheckDecimalType();
    bool IsDigit(const char& c);
    bool IsOctalDigit(const char& c);
    bool IsBinDigit(const char& c);
    bool LexIntegerSuffix();
    bool LexFloatSuffix();
    bool IsAlpha(const char& c);
    bool IsHexDigit(const char& c);
    int64_t IsUniversalChar(const char* c, size_t maxPossibleLen);
    bool IsNonDigit(const char* c, size_t maxPossibleLen);
    bool IsAlphaDigitFloor(const char& c);

public:
    FILE_STATE mainFile;
    FileManager* manager;
    std::stack<FilePos> files;
    std::deque<char> charsQueue;
    std::queue<SourceLocation> currLocations;
    const char* fEnd;
    const char* fCurr;
    std::unordered_map<std::string_view, TokenType::Type> keywordsMap;
    const CompilationOpts* opts;
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
    
    SourceLocation(size_t id,  int64_t offset, int64_t line, int64_t len) :
    id(FILE_ID{id}), offset(offset), line(line), len(len)
    {};

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
    uint8_t skippedHorizWhitespace : 1;
};

struct DecimalType
{
    int8_t len;
    int8_t type;
};