#pragma once
#include "AstNode.hpp"
#include "SemaTypes.hpp"
#include <unordered_map>

struct SemanticAnalyzer
{
    SemanticAnalyzer(FileManager* manager);
    void Analyze(const Ast::Node* root);
    void AnalyzeDeclaration(const Ast::Node* declSpecs, const Ast::Node* initDeclList);
    void AnalyzeTypedef(const Ast::Node* declSpecs, const Ast::Node* initDeclList);
    void AnalyzeUnion(const Ast::Node* unionTree, DeclSpecs* spec);
    void AnalyzeStruct(const Ast::Node* structTree, DeclSpecs* spec);
    void AnalyzeEnum(const Ast::Node* enumTree, DeclSpecs* spec);
    void AnalyzeSimpleType(const Ast::Node* typeSequence, DeclSpecs* spec);
    bool AliasOfType(const std::string_view identifier);
    DeclSpecs AnalyzeDeclSpec(const Ast::Node* declSpecs);
    std::string_view AddSymbolName(const char* name);
    std::string_view GetViewForToken(const Token &token);

    std::unordered_map<std::string_view, Symbol*> symbolTable;
    PagedBuffer symNameBuff;
    FileManager* manager;
    std::string compoundTypeStr;
};

