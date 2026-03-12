#pragma once
#include "AstNode.hpp"
#include "../utils/FileManager.hpp"
#include "Preprocessor.hpp"

struct ParsingState
{
    uint16_t parsingConstantExpr : 1;
};

struct Parser
{
    Parser(FILE_STATE mainFile, FileManager* manager, const CompilationOpts* opts);
    void Parse();
    // misc ops
    Token GetCurrToken();
    void PutBackAtFront(Token token);
    void ConsumeToken();
    void ConsumeExpectedToken(TokenType::Type type);
    void IssueWarning(const Token* token, const char* errMsg, ...);
    void IssueWarning(const FILE_ID* fileId, const SourceLocation* loc, const char* errMsg, va_list args);
    // node management
    Ast::Node* AllocateAstNodes(uint16_t count = 1);
    void AddNodePage();
    // checks
    bool IsAssignment(Token* token);
    bool IsAssignment(TokenType::Type type);
    // expression parsing
    Ast::Node* ParseConstantExpr();
    Ast::Node* ParseExpression();
    Ast::Node* AssignmentExpression();
    Ast::Node* ConditionalExpression();
    Ast::Node* LogicalOrExpression();
    Ast::Node* LogicalAndExpression();
    Ast::Node* InclusiveOrExpression();
    Ast::Node* ExclusiveOrExpression();
    Ast::Node* AndExpression();
    Ast::Node* EqualityExpression();
    Ast::Node* RelationalExpression();
    Ast::Node* ShiftExpression();
    Ast::Node* AdditiveExpression();
    Ast::Node* MultiplicativeExpression();
    Ast::Node* CastExpression();
    Ast::Node* UnaryExpression();
    Ast::Node* PostfixExpression();
    Ast::Node* PrimaryExpression();
    Ast::Node* ParseIdentifier();

    FileManager* manager;
    Preprocessor PP;
    const CompilationOpts* opts;
    std::deque<Token> tokenQueue;
    Ast::Node* unaryHandle;
    PagedBuffer nodeBuffer;

    ParsingState pState;
};
