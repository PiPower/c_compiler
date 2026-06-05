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
// special variable idx values
constexpr int64_t NOT_EMITTED = -2;
constexpr int64_t ANON_EMITTED = -3;

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
    // General, do not change buffer type
    void EmitTypename(SymbolType* symType, const std::string_view& typeName, bool useQueue = true);
    void EmitBuiltInTypename(const std::string_view& builInType);
    bool EmitDeclarator(const AccessArray* acc, const std::string_view* typeName);
    bool EmitDeclaratorAcc(const AccessArray* acc, const std::string_view* typeName);
    void EmitMember(Member* member);
    bool EmitAccessArrayOpened(const AccessArray* accArr, uint64_t* bracket);
    // Type stuff
    void EmitUnionStruct(SymbolType* symType, const std::string_view& name, bool flushQueue = true);
    // Function stuff
    void EmitFunctionName(const DeclSpecs* spec, const Declarator* decl);
    void EmitFunctionClose();
    // Global stuff 
    void EmitGlobalVariable(const DeclSpecs* spec, const Declarator* decl);
    void EmitGlobalBuiltInInit(const Ast::Node* initExpr, uint32_t alignment);
    void ZeroInitGlobalVar(const DeclSpecs* spec, const Declarator* decl);
    void EmitGLobalArrayAlignment(bool isPtr, uint32_t alignment);
    void EmitInitializer(const DeclSpecs* spec, const Ast::Node* initializer, bool isComplexType);
    // Local stuff
    void EmitLocalVariable(const SymbolVariable* symVar);
    void InitLocalArray(const std::string_view& arrName, const AccessArray* accArr, const Ast::Node* initExpr, const DeclSpecs *spec);
    void EmitLocalStorage(BuiltIn::Type type, int32_t alignment, int64_t destIdx, int64_t srcIdx);
    void EmitLocalConstAsm(BuiltIn::Type type, int32_t alignment, int64_t destIdx, const Typed::Number& num);
    // string sutff
    int64_t EmitString(const Ast::Node* string);

    // misc
    std::string_view GetViewForToken(const Token &token);
    void AddSymbolToEmitQueue(SymbolType* symType, const std::string_view& name);
    void FlushTypeQueue();
    void WriteToFile(int fd);
    int64_t GetIdxForLocalVar();
    void StartArray();
    void EndArray();
    void ArgSeparator();
    // expressions

    // data writing
    void PushBufferType();
    void PopBufferType();
    void BindTypeBuffer();
    void BindStrBuffer();
    void BindFuncBuffer();
    void BindGlobalVarBuffer();
    void BindLocalBuffer();
    void WriteByte(const char* c);
    void WriteByte(char c);
    void WriteCharData(const char* data, ...);
    void CopyBuffers(uint8_t dest, uint8_t src);
    void ResetBuffer(uint8_t buff);

    std::unordered_map<std::string_view, int> typeCounter;
    std::unordered_map<SymbolType*, LlvmType> emittedTypes;
    std::queue<PendingType> typeQueue;
    uint8_t chosenBuffer;
    std::array<std::vector<char*>, 5> writableBufferArr;
    std::array<std::vector<char*>, 5> allocatedBufferHandles;
    std::array<char*, 5> currPtrArr;
    FunctionContext currFn;
    std::stack<uint8_t> buffStack;

    PagedHeap typeHeap;
    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor* nodeExec;
    Logger logger;
};
