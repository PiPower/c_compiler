#pragma once
#include "AstNode.hpp"
#include "../LangTypes.hpp"
#include <unordered_map>
#include "Preprocessor.hpp"
#include "CodeGen.hpp"
constexpr size_t  POINTER_SIZE = 8;
/*
    sema supports anonymous struct members
*/
struct SemanticAnalyzer
{
    SemanticAnalyzer(FileManager* manager, SymbolTable* symTab);
    void Analyze(const Ast::Node* root);
    void AnalyzeDeclaration(const Ast::Node* declSpecs, const Ast::Node* initDeclList);
    void AnalyzeFunctionDef(const Ast::Node* body, const Ast::Node* decl);
    void AnalyzeFunctionDecl(DeclSpecs* spec, Declarator* decl);
    Declarator ProcessDecl(const Ast::Node* declarator, std::stack<const Ast::Node*>* accessTypes, uint8_t forbbidenAccTypes);
    Declarator ProcessAbstractDecl(const Ast::Node* abstDecl, std::stack<const Ast::Node*>* accessTypes, uint8_t forbbidenAccTypes);
    FunctionParams* ProcessFnParams(const Ast::Node* paramsNode, size_t* paramCount);
    StructDeclaration AnalyzeStructDeclaration(const Ast::Node* declSpecs, const Ast::Node* structDeclList);
    void AnalyzeTypedef(DeclSpecs* declSpec, const Ast::Node* initDeclList);
    void AnalyzeStructUnion(const Ast::Node* structTree, DeclSpecs* spec, bool isStruct);
    void AnalyzeInitDeclList(DeclSpecs* declSpec, const Ast::Node* initDeclList);
    Declarator AnalyzeDeclarator(const Ast::Node* declarator, uint8_t forbbidenAccTypes = 0);
    void AnalyzeEnum(const Ast::Node* enumTree, DeclSpecs* spec);
    int BuiltInBitCount(BuiltIn::Type type);
    BuiltIn::Type BitCountToIntegerType(uint8_t BitCount, bool isSigned);
    void AnalyzeSimpleType(const Ast::Node* typeSequence, DeclSpecs* spec);
    bool NamesAType(const std::string_view& identifier);
    uint64_t GetAnnonymousStructId();
    uint64_t GetAnnonymousUnionId();
    bool IsMemberPointer(const Member* member);
    std::string_view SimpleTypeToString(BuiltIn::Type type);
    MemoryDesc GetMemberDesc(AccessType* accType, const std::string_view& typeName);
    DeclSpecs AnalyzeDeclSpec(const Ast::Node* declSpecs);
    // misc
    std::string_view GetViewForToken(const Token &token);
    void WriteCodeToFile(const char* filename);
    bool CompareParams(size_t paramCount, const FunctionParams* p1, const FunctionParams* p2);
    bool CompareDeclSpec(const DeclSpecs* s1, const DeclSpecs* s2);
    bool CompareDeclarators(const Declarator* d1, const Declarator* d2);

    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor ne;
    CodeGen codeGen;
    // used as local string to avoid constant re allocation
    // it is not guaranted to be valid after call to any SEMA function
    Logger logger;
};


