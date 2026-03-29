#pragma once
#include "AstNode.hpp"
#include "SemaTypes.hpp"
#include <unordered_map>
#include "Preprocessor.hpp"

struct SemanticAnalyzer
{
    SemanticAnalyzer(FileManager* manager, SymbolTable* symTab);
    void Analyze(const Ast::Node* root);
    void AnalyzeDeclaration(const Ast::Node* declSpecs, const Ast::Node* initDeclList);
    StructDeclaration AnalyzeStructDeclaration(const Ast::Node* declSpecs, const Ast::Node* structDeclList);
    void AnalyzeTypedef(DeclSpecs* declSpec, const Ast::Node* initDeclList);
    void AnalyzeUnion(const Ast::Node* unionTree, DeclSpecs* spec);
    void AnalyzeStruct(const Ast::Node* structTree, DeclSpecs* spec);
    InitDeclarator AnalyzeDeclarator(const Ast::Node* declarator, const Ast::Node* initializer);
    void AnalyzeEnum(const Ast::Node* enumTree, DeclSpecs* spec);
    void AnalyzeSimpleType(const Ast::Node* typeSequence, DeclSpecs* spec);
    bool IsAliasOfType(const std::string_view& identifier);
    DeclSpecs AnalyzeDeclSpec(const Ast::Node* declSpecs);
    // misc
    std::string_view GetViewForToken(const Token &token);

    SymbolTable* symTab;
    FileManager* manager;
    // used as a handle to memory for simple type name
    std::string compoundTypeStr;
};


