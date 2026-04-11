#pragma once
#include "AstNode.hpp"
#include "../utils/FileManager.hpp"
#include "Preprocessor.hpp"
#include "SemanticAnalysis.hpp"

struct ParsingState
{
    uint16_t parsingConstantExpr : 1;
};

struct Parser
{
    Parser(FILE_ID mainFileId, SemanticAnalyzer* analyzer, FileManager* manager, const CompilationOpts* opts);
    Ast::Node* Parse();
    // misc ops
    Token GetCurrToken();
    Token GetCurrTokenInternal();
    void PutBackAtFront(Token token);
    void ConsumeToken();
    std::string_view GetViewForToken(const Token &token);
    void ConsumeExpectedToken(TokenType::Type type);
    void IssueWarning(const Token* token, const char* errMsg, ...);
    void IssueWarning(const FILE_ID* fileId, const SourceLocation* loc, const char* errMsg, ...);
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
    Ast::Node* AssignmentExpression(Ast::Node* unaryExpr = nullptr);
    Ast::NodeType GetAssignmentType(TokenType::Type asmType);
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
    // statements
    Ast::Node* ParseCompoundStatement();
    Ast::Node* ParseStatement();
    // declaration parsing
    Ast::Node* ParseDeclaration(bool consumeSemicolon);
    Ast::Node* ParseInitializer();
    Ast::Node* ParseInitDeclList();
    Ast::Node* ParseDeclarator();
    Ast::Node* ParsePointer();
    Ast::Node* ParseFunctionCallArgs();
    Ast::Node* ParseArrayArgs();
    Ast::Node* ParseDirectDeclarator();
    Ast::Node* ParseDeclSpec();
    Ast::Node* DeclSpecSubtype(bool* allowTypeSpec);
    Ast::Node* StorageSpec();
    Ast::Node* InitializerList();
    Ast::Node* TypeSpecifier();
    Ast::Node* TypeName();
    Ast::Node* TypeQualifierList();
    Ast::Node* FunctionSpec();
    Ast::Node* StructOrUnionSpec();
    Ast::Node* EnumSpec();
    Ast::Node* StructDeclaration();
    Ast::Node* StructDeclarator();
    Ast::Node* AbstractDeclarator();
    Ast::Node* ParameterDecl();
    Ast::Node* ParseDirectAbstractDeclarator();
    Ast::Node* ParameterTypeList();
    
    SemanticAnalyzer* analyzer;
    FileManager* manager;
    Preprocessor PP;
    const CompilationOpts* opts;
    std::deque<Token> tokenQueue;
    std::stack<Ast::Node*> unaryHandles;
    PagedBuffer nodeBuffer;
    SourceLocation currentLocation;
    ParsingState pState;
};
