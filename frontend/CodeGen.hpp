#pragma once
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../SymbolTable.hpp"
#include "../utils/PagedHeap.hpp"
#include "../utils/FileManager.hpp"
#include "NodeExecutor.hpp"
#include <stdarg.h>
#include "../utils/Misc.hpp"
constexpr uint64_t CG_EMPTY_ARRAY = 0x0;
constexpr uint64_t CG_ZERO_SIZED_ARRAY = 0xFFFFFFFFFFFFFFFF;

struct LlvmType
{
    int symbolSaveCounter;
    bool isEmitted;
};

struct PendingType
{
    SymbolType* symType;
    const std::string_view name;
};

struct FunctionContext
{
    bool inFunction;
    int64_t variableIdx;
    std::string_view fnName;
};

struct CodeGen
{
    CodeGen(SymbolTable* symTab,  FileManager* manager, NodeExecutor* ne);
    void EmitUnionStruct(SymbolType* symType, const std::string_view& name, bool flushQueue = true);
    void EmitTypename(SymbolType* symType, const std::string_view& typeName, bool useQueue = true);
    void EmitBuiltInTypename(SymbolType* symType);
    std::string_view GetBuiltInName(SymbolType* symType);
    bool EmitDeclarator(const AccessArray* acc, const std::string_view* typeName);
    bool EmitDeclaratorAcc(const AccessArray* acc, const std::string_view* typeName);
    void EmitMember(Member* member);
    bool EmitAccessArrayOpened(const AccessArray* accArr, uint64_t* bracket);
    void EmitGlobalVariable(const DeclSpecs* spec, const Declarator* decl);
    void EmitLocalVariable(const DeclSpecs* spec, const Declarator* decl);
    void EmitFunctionName(const DeclSpecs* spec, const Declarator* decl);
    void EmitInitializer(const DeclSpecs* spec, const Ast::Node* intializer);
    void EmitFunctionClose();
    void InitGlobalVar(const DeclSpecs* spec, const Declarator* decl, bool isPtr);
    void ZeroInitGlobalVar(const DeclSpecs* spec, const Declarator* decl);
    void InitGlobalArray(const AccessArray* accArr, const Ast::Node* initExpr, const DeclSpecs *spec);
    void EmitInitGlobalArray(const AccessArray *accArr, const DeclSpecs *spec, const ArrayInitPair* initialzier);
    std::string_view GetViewForToken(const Token &token);
    void AddSymbolToEmitQueue(SymbolType* symType, const std::string_view& name);
    void FlushTypeQueue();
    void WriteToFile(int fd);
    int64_t GetIdxForLocalVar();

    void BindTypeBuffer();
    void BindFuncBuffer();
    void BindGlobalVarBuffer();
    void BindLocalVarBuffer();

    void WriteByte(const char* c);
    void WriteByte(char c);
    void WriteCharData(const char* data, ...);

    std::unordered_map<std::string_view, int> typeCounter;
    std::unordered_map<SymbolType*, LlvmType> emittedTypes;
    std::queue<PendingType> typeQueue;
    uint8_t chosenBuffer;
    std::array<std::vector<char*>, 4> writableBufferArr;
    std::array<char*, 4> currPtrArr;
    std::vector<char*> localBufferHandle;
    FunctionContext currFn;

    PagedHeap utilHeap;
    PagedHeap typeHeap;
    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor* nodeExec;
    Logger logger;
};
