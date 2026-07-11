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
    int64_t variableIdx;
    std::string_view fnName;
    bool inFunction;
    bool isBlockTerminated;
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
// funciton param flags
constexpr int8_t fpIsLast = 0x01;
constexpr int8_t fpIsUsedInCall = 0x01 << 1;

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
    bool IsBlockTerminated();
    // Type stuff
    void EmitUnionStruct(SymbolType* symType, const std::string_view& name, bool flushQueue = true);
    // Function stuff
    void EmitFunctionName(const DeclSpecs* spec, const Declarator* decl, bool declareFunc);
    void EmitReturnByPtr(SymbolType* symType, const std::string_view& typenameView, int8_t flags, int64_t argIdx = EXPR_ID_IGNORE);
    void EmitFunctionParam(BuiltIn::Type type, int8_t flags, Operator op);
    void EmitFunctionParam(SymbolType* symType, const std::string_view& typeName, int8_t flags, int64_t idx);
    void CloseParamList(bool isDeclaration);
    void EmitFunctionBodyStart();
    void EmitFunctionBodyClose(BuiltIn::Type retType, int64_t retIdx, int64_t retVal, DeclSpecs* retSpec);
    // Global stuff 
    void EmitGlobalVariable(const DeclSpecs* spec, const Declarator* decl);
    void EmitGlobalBuiltInInit(const Ast::Node* initExpr, uint32_t alignment);
    void ZeroInitGlobalVar(const DeclSpecs* spec, const Declarator* decl);
    void EmitGLobalArrayAlignment(bool isPtr, uint32_t alignment);
    void EmitInitializer(const DeclSpecs* spec, const Ast::Node* initializer, bool isComplexType);
    // Local stuff
    int64_t AllocatePassByTmpStruct(const std::string_view& left, const std::string_view& right, uint64_t alignment);
    void EmitSimpleReturn(BuiltIn::Type dType, Operator ret);
    void EmitLocalVariable(const SymbolVariable* symVar);
    int64_t AllocateLocalVariable(BuiltIn::Type type, SymbolType* symType = nullptr, const std::string_view& typeName = "");
    void InitLocalArray(const std::string_view& arrName, const AccessArray* accArr, const Ast::Node* initExpr, const DeclSpecs *spec);
    void EmitLocalBuiltInStorage(BuiltIn::Type type, int32_t alignment, int64_t destIdx, Operator op);
    void EmitLocalStorage(BuiltIn::Type type, int32_t alignment, int64_t destIdx, int64_t srcIdx);
    void EmitLocalStrStorage(int64_t destIdx, int64_t strIdx);
    void EmitLocalNullStorage(int64_t destIdx);
    void EmitLocalNamedStore(BuiltIn::Type type, int32_t alignment, int64_t dstIdx, const std::string_view& name);
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
        const std::string_view& typeName, 
        int64_t arrayIdx, 
        const std::vector<uint64_t>& indicies);

    void EmitLocalLabel(int64_t idx);
    void EmitLocalJump(int64_t label);
    void EmitLocalCondJump(int64_t cond, int64_t jmpIfTrue, int64_t jmpIfFalse);
    int64_t EmitLocalLabel();
    void EmitLocalSwitch(
        BuiltIn::Type type, 
        int64_t cond, 
        int64_t exitLabel, 
        const std::vector<int64_t>& caseLabels,
        const std::vector<Typed::Number>& labelValues);

    int64_t EmitLocalGlLoad(BuiltIn::Type type, int32_t alignment, const std::string_view& varName);
    int64_t EmitLocalLoad(BuiltIn::Type type, int32_t alignment, int64_t loadIdx);
    int64_t EmitLocalLoad(const std::string_view& typeView, int32_t alignment, int64_t loadIdx);
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
    int64_t EmitLocalCmpGe(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalCmpGeEq(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalCmpLe(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalCmpLeEq(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalCmpEq(BuiltIn::Type opType, Operator left, Operator right);
    int64_t EmitLocalCmpNotEq(BuiltIn::Type opType, Operator left, Operator right);
    // fuction call related
    int64_t EmitOpenFnCall(BuiltIn::Type ret, std::string_view fnName, const DeclSpecs* spec = nullptr);
    void EmitCloseFnCall();
    // variable related
    void EmitZeroInitType(bool isGlobal);
    void EmitZeroInitInt(bool isGlobal);
    void EmitZeroInitFloat(bool isGlobal);
    void EmitString(bool isGlobal, int64_t strIdx);
    void EmitConstant(bool isGlobal, BuiltIn::Type dstType, const Typed::Number &num);
    // general functions
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
    //intrinsics
    void EmitLocalIntMemcpy(uint64_t lAlign, uint64_t rAlign, int64_t dest, int64_t src, uint64_t size);
    // misc
    void AddSymbolToEmitQueue(SymbolType* symType, const std::string_view& name);
    void FlushTypeQueue();
    void WriteToFile(int fd);
    int64_t GetIdxForLocalVar();
    void StartArray();
    void EndArray();
    void ArgSeparator();

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

    std::unordered_map<std::string_view, int64_t> emittedStrings;
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
    int64_t declareAttribute;

    PagedHeap typeHeap;
    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor* nodeExec;
    Logger logger;
};
