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
#include "Expression.hpp"
constexpr uint64_t CG_EMPTY_ARRAY = 0x0;
constexpr uint64_t CG_ZERO_SIZED_ARRAY = 0xFFFFFFFFFFFFFFFF;
// special variable idx values
constexpr int64_t NOT_EMITTED = -2;
constexpr int64_t ANON_EMITTED = -3;
constexpr int64_t INDEX_INVALID = -1;
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

struct Operator
{
    int64_t idx;
    Typed::Number num;
};

enum class Intrinsic
{
    llvm_memcpy
};

struct ByValueStructDesc
{
    std::string lType;
    std::string rType;
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
    void EmitReturnByPtr(SymbolType* symType, const std::string_view& typenameView);
    void EmitFunctionParam(BuiltIn::Type type, bool lastParam, int64_t idx);
    void EmitFunctionParam(SymbolType* symType, const std::string_view& typeName, bool lastParam, int64_t idx);
    void CloseParamList();
    void EmitFunctionClose();
    // Global stuff 
    void EmitGlobalVariable(const DeclSpecs* spec, const Declarator* decl);
    void EmitGlobalBuiltInInit(const Ast::Node* initExpr, uint32_t alignment);
    void ZeroInitGlobalVar(const DeclSpecs* spec, const Declarator* decl);
    void EmitGLobalArrayAlignment(bool isPtr, uint32_t alignment);
    void EmitInitializer(const DeclSpecs* spec, const Ast::Node* initializer, bool isComplexType);
    // Local stuff
    int64_t AllocatePassByTmpStruct(BuiltIn::Type left, BuiltIn::Type right, uint64_t alignment);
    void EmitSimpleReturn(BuiltIn::Type dType, Operator ret);
    void EmitLocalVariable(const SymbolVariable* symVar);
    void InitLocalArray(const std::string_view& arrName, const AccessArray* accArr, const Ast::Node* initExpr, const DeclSpecs *spec);
    void EmitLocalStorage(BuiltIn::Type type, int32_t alignment, int64_t destIdx, int64_t srcIdx);
    void EmitLocalStrStorage(int64_t destIdx, int64_t strIdx);
    void EmitLocalNullStorage(int64_t destIdx);
    void EmitLocalNamedStore(BuiltIn::Type type, int32_t alignment, int64_t dstIdx, const std::string_view name);
    void EmitLocalConstAsm(BuiltIn::Type type, int32_t alignment, int64_t destIdx, const Typed::Number& num);
    void CopyPassTmpStructToStruct(
        int64_t destIdx, 
        uint64_t destSize,
        BuiltIn::Type left, 
        BuiltIn::Type right, 
        uint64_t alignment,
        int64_t lIdx, 
        int64_t rIdx);
    int64_t EmitLocalArrGetElemPtr(
        const AccessArray* acc, 
        const std::string_view* typeName, 
        int64_t arrayIdx, 
        const std::vector<uint64_t>* indicies);
    int64_t EmitLocalLoad(BuiltIn::Type type, int32_t alignment, int64_t loadIdx);
    int64_t EmitLocalSignExt(BuiltIn::Type dstType, BuiltIn::Type srcType, int64_t loadIdx);
    int64_t EmitLocalZeroExt(BuiltIn::Type dstType, BuiltIn::Type srcType, int64_t loadIdx);
    int64_t EmitLocalAddition(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalSubtraction(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalMultiplication(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalDivision(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalModulus(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalBitAnd(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalBitOr(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalBitXor(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalShiftLeft(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalShiftRight(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalIntTruncate(BuiltIn::Type dstType, BuiltIn::Type srcType, Operator src);

    void EmitZeroInitType(bool isGlobal);
    void EmitZeroInitInt(bool isGlobal);
    void EmitZeroInitFloat(bool isGlobal);
    void EmitString(bool isGlobal,int64_t strIdx);
    void EmitConstant(bool isGlobal, BuiltIn::Type dstType, const Typed::Number &num);

    int64_t EmitLocalBinaryOp(
        BuiltIn::Type opType, 
        Operator left, 
        Operator right,
        std::string_view opSigned,
        std::string_view opUnsigned,
        std::string_view opFloat,
        bool usePoison);
    // string sutff
    int64_t EmitString(const Ast::Node* string);
    // misc
    void AddSymbolToEmitQueue(SymbolType* symType, const std::string_view& name);
    void FlushTypeQueue();
    void WriteToFile(int fd);
    int64_t GetIdxForLocalVar();
    void StartArray();
    void EndArray();
    void ArgSeparator();

    std::string_view MapBuiltInToLlvm(BuiltIn::Type srcType);
    ByValueStructDesc BuildValueStruct(const StructDesc& desc);
    std::string getRetName(const DeclSpecs* spec, const Declarator* decl);
    // expressions

    // data writing
    void PushBufferType();
    void PopBufferType();
    void BindTypeBuffer();
    void BindStrBuffer();
    void BindFuncBuffer();
    void BindGlobalVarBuffer();
    void BindLocalBuffer();
    void BindIntrinsicBuffer();
    void BindAttrBuffer();

    void DeclareIntrinsic(Intrinsic intr);
    void WriteByte(const char* c);
    void WriteByte(char c);
    void WriteByteInHex(char c);
    void WriteCharData(const char* data, ...);
    void CopyBuffers(uint8_t dest, uint8_t src);
    void ResetBuffer(uint8_t buff);

    std::unordered_map<std::string_view, int> typeCounter;
    std::unordered_map<SymbolType*, LlvmType> emittedTypes;
    std::queue<PendingType> typeQueue;
    uint8_t chosenBuffer;
    std::array<std::vector<char*>, 7> writableBufferArr;
    std::array<std::vector<char*>, 7> allocatedBufferHandles;
    std::array<char*, 7> currPtrArr;
    FunctionContext currFn;
    std::stack<uint8_t> buffStack;
    int64_t attrCtr;

    PagedHeap typeHeap;
    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor* nodeExec;
    Logger logger;
};
