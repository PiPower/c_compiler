#pragma once
#include "AstNode.hpp"
#include "../LangTypes.hpp"
#include <unordered_map>
#include "Preprocessor.hpp"
#include "CodeGen.hpp"
#include "Expression.hpp"
constexpr size_t  POINTER_SIZE = 8;


struct SemanticAnalyzer
{
    SemanticAnalyzer(FileManager* manager, SymbolTable* symTab);
    void Analyze(const Ast::Node* root);
    void AnalyzeDeclaration(const Ast::Node* declSpecs, const Ast::Node* initDeclList);
    void AnalyzeFunctionDef(const Ast::Node* body, const Ast::Node* decl);
    void AnalyzeFunctionDecl(DeclSpecs* spec, Declarator* decl);
    Declarator ProcessDecl(const Ast::Node* declarator, std::stack<const Ast::Node*>* accessTypes, bool isAbstract, const AccessArray* typedefAcc);
    FunctionParams* ProcessFnParams(const Ast::Node* paramsNode, size_t* paramCount);
    StructDeclaration AnalyzeStructDeclaration(const Ast::Node* declSpecs, const Ast::Node* structDeclList);
    void AnalyzeTypedef(DeclSpecs* declSpec, const Ast::Node* initDeclList);
    void AnalyzeStructUnion(const Ast::Node* structTree, DeclSpecs* spec, bool isStruct);
    void AnalyzeInitDeclList(DeclSpecs* declSpec, const Ast::Node* initDeclList);
    Declarator AnalyzeDeclarator(const Ast::Node* declarator, const AccessArray* typedefAcc, const Ast::Node *initExpr);
    void AnalyzeEnum(const Ast::Node* enumTree, DeclSpecs* spec);
    void DeduceInferableArrSize(Declarator* decl);
    int BuiltInBitCount(BuiltIn::Type type);
    BuiltIn::Type BitCountToIntegerType(uint8_t BitCount, bool isSigned);
    void AnalyzeSimpleType(const Ast::Node* typeSequence, DeclSpecs* spec);
    bool NamesAType(const std::string_view& identifier);
    void AnalyzeVariableDecl(const DeclSpecs* spec, const Declarator* decl);
    void AnalyzeGlobalVarDecl(const DeclSpecs* spec, const Declarator* decl);
    void InitGlobalVar(const SymbolVariable* symVar);
    void InitArray(
        const AccessArray* accArr, 
        const Ast::Node* initExpr, 
        const SymbolVariable* symVar, 
        std::vector<uint64_t>* parentPosition);

    void AnalyzeLocalVarDecl(const DeclSpecs* spec, const Declarator* decl);
    uint64_t GetAnnonymousStructId();
    uint64_t GetAnnonymousUnionId();
    bool IsMemberPointer(const Member* member);
    std::string_view SimpleTypeToString(BuiltIn::Type type);
    DeclSpecs AnalyzeDeclSpec(const Ast::Node* declSpecs);
    void InitLocalVariable(const SymbolVariable* symVar);
    ExprRet ResolveAssignment(ExprRet dst, ExprRet src);
    // expressions
    ExprRet AnalyzeInitializer(bool isGlobal, const DeclSpecs *spec, const AccessArray *accArr, const Ast::Node *initializer, bool isComplexType);
    ExprRet AnalyzeExpr(const Ast::Node* root);
    ExprRet CompoundLiteral(const Ast::Node* literal);
    ExprRet LoadCharacter(const Ast::Node* character);
    ExprRet LoadConstant(const Ast::Node* constant);
    ExprRet LoadStringLiteral(const Ast::Node* string);
    ExprRet HandleInitExpr(const Ast::Node* root);
    ExprRet HandleCast(const Ast::Node* root);
    ExprRet HandleOpMinus(const Ast::Node* root);
    ExprRet HandleAssignment(const Ast::Node* root);
    ExprRet HandleSimpleAssignment(const ExprRet* dst, const ExprRet* src);
    ExprRet HandlePointerAssignment(const ExprRet* dst, const ExprRet* src);
    ExprRet HandleGetAddr(const Ast::Node* root);
    ExprRet HandleIdentifier(const Ast::Node* root);
    void HandleTypePromotion(const ExprRet* left, const ExprRet* right, ExprRet* outLeft, ExprRet* outRight);
    ExprRet HandleTypeExtension(const ExprRet* src, BuiltIn::Type newType);

    // misc
    void WriteCodeToFile(const char* filename);
    bool CompareParams(size_t paramCount, const FunctionParams* p1, const FunctionParams* p2);
    bool CompareDeclSpec(const DeclSpecs* s1, const DeclSpecs* s2);
    bool CompareDeclarators(const Declarator* d1, const Declarator* d2);
    void EmitUninitializedGlobals();
    void ResolveIntegralPromotion(const ExprRet* left, const ExprRet* right, BuiltIn::Type* outLeft, BuiltIn::Type* outRight);
    int GetIntRank(BuiltIn::Type type);
    void BinaryExprProlog(ExprRet* left, ExprRet* right, const Ast::Node* leftTerm, const Ast::Node* rightTerm);

    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor ne;
    CodeGen codeGen;
    std::unordered_set<SymbolVariable*> uninitGlobals;
    // used as local string to avoid constant re allocation
    // it is not guaranted to be valid after call to any SEMA function
    Logger logger;
    PagedHeap utilHeap;
};


