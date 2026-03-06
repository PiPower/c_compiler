#pragma once
#include "Lexer.hpp"

struct Preprocessor
{
    Preprocessor(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts);
    int32_t Peek(Token* token);
    int32_t ExecuteDirective(Token* token);
private:
    void IssueWarning(const FILE_ID* fileId, const SourceLocation* loc, const char* errMsg, ...);
    Token GetCurrToken();
    void ConsumeToken();
    void ConsumeExpectedToken(TokenType::Type  type);
    std::string_view FormHeadername();
    int32_t HandleIf();
    int32_t HandleElse();
    int32_t HandleInclude();
    int32_t HandleDefine();
    int32_t HandleIfdef();
    int32_t HandleIfndef();
    int32_t HandleElif();
    int32_t HandleEndif();
    int32_t HandleLine();
    int32_t HandleError();
    int32_t HandlePragma();
public:
    Lexer lexer;
    FileManager* manager;
    const CompilationOpts* opts;
    std::deque<Token> tokenQueue;
};
