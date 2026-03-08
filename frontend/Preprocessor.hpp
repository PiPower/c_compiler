#pragma once
#include "Lexer.hpp"
#include "AstNode.hpp"
#include "TypedNumber.hpp"
struct PreprocessorStages
{
    uint16_t If : 1;
    uint16_t Ifdef : 1;
    uint16_t Ifndef : 1;

};

struct Macro
{
    std::vector<Token> tokenList;
};

struct ConditionalBlock
{
    int64_t nestLevel;
    bool doneIncluding;
};

struct Preprocessor
{
    Preprocessor(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts);
    int32_t Peek(Token* token);
    void ExecuteConstantExpr(Ast::Node* expr);
private:
    Typed::Number ExecuteNode(Ast::Node* expr);
    int32_t ExecuteDirective(Token* token);
    void IssueWarning(const FILE_ID* fileId, const SourceLocation* loc, const char* errMsg, ...);
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
    int32_t HandlePragma(); 
    int32_t HandleUndef();
    int32_t SkipTokensInBlock(Token* infoToken = nullptr);
    ConditionalBlock CreateBlock();
public:
    Lexer lexer;
    FileManager* manager;
    const CompilationOpts* opts;
    std::deque<Token> tokenQueue;
    PreprocessorStages stages;
    std::stack<ConditionalBlock> conditionalBlocks;
    std::unordered_map<std::string_view, Macro> macros;
    std::vector<Ast::Node*> constantNodes;
};
