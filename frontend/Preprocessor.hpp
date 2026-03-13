#pragma once
#include "Lexer.hpp"
#include "AstNode.hpp"
#include "TypedNumber.hpp"
struct PreprocessorStages
{
    uint16_t If : 1; // informs that currently we are inside a macro
    uint16_t ConstantExpr : 1;
    uint16_t eofTriggered : 1;
};

struct MacroFlags
{
    uint8_t callable : 1;
};
struct Macro
{
    std::vector<Token> tokenList;
    MacroFlags flags;
};

struct ConditionalBlock
{
    int64_t nestLevel;
    bool doneIncluding;
};

using MacroMapIter = std::unordered_map<std::string_view, Macro>::iterator;
struct Preprocessor
{
    Preprocessor(FILE_ID mainFileId, FileManager* manager, const CompilationOpts* opts);
    int32_t Peek(Token* token);
    void ExecuteConstantExpr(Ast::Node* expr);
    Typed::Number ExecuteNode(Ast::Node* expr);
private:
    bool FetchMacro(const std::string_view macroName, Macro** macro);
    bool FetchMacro(const std::string_view* macroName, Macro** macro);
    void PushInitFile();
    void FillQueueWithMacro(const Macro* macro);
    uint8_t GetTokenMode(const Token& token);
    const char* GetDataPtr(const Token* token);
    int32_t ExecuteDirective(Token* token);
    void IssueWarning(const Token* token, const char* errMsg, ...);
    void IssueWarning(const FILE_ID* fileId, const SourceLocation* loc, const char* errMsg, va_list args);
    std::string_view GetViewForToken(const Token& token);
    Token GetCurrToken();
    void PutBackAtFront(Token token);
    void ConsumeToken();
    void ConsumeExpectedToken(TokenType::Type type);
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
    int32_t HandleDefined();
    int32_t HandlePragma(); 
    int32_t HandleUndef();
    int32_t SkipTokensInBlock(Token* infoToken = nullptr);
    ConditionalBlock CreateBlock();
    SourceLocation GetZeroLocation();
    SourceLocation GetOneLocation();
    bool ProcessDefined();
public:
    Lexer lexer;
    FileManager* manager;
    const CompilationOpts* opts;
    std::deque<Token> tokenQueue;
    PreprocessorStages stages;
    std::stack<ConditionalBlock> conditionalBlocks;
    std::unordered_map<std::string_view, Macro> macros;
    std::vector<Ast::Node*> constantNodes;
    uint8_t blockResult;
    FILE_ID preprocessorFile;
};
