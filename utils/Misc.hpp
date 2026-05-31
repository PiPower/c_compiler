#pragma once
#include "../LangTypes.hpp"
#include "../frontend/NodeExecutor.hpp"
#include "PagedHeap.hpp"

struct ArrayInitPair
{
    uint64_t idx;
    const Ast::Node* initializerList;
};

struct ArraySize
{
    uint64_t elementCount;
    bool hitPointer;
};

struct MemoryDesc
{
    uint64_t size;
    uint32_t alignment;
};

constexpr inline Ast::Node* lenToAstPtr(size_t len)
{
    return reinterpret_cast<Ast::Node*>(static_cast<uintptr_t>(len));
}

constexpr inline size_t astPtrToLen(Ast::Node* node)
{
    return static_cast<size_t>(reinterpret_cast<uintptr_t>(node));
}

Ast::Node* BuildInitializerList(const std::vector<Ast::Node>& initItems, PagedHeap* allocator);
ArraySize GetArrayElemCount(const AccessArray *accArray, Logger* logger, NodeExecutor* ne);
MemoryDesc GetMemoryDesc(const AccessArray *accArray, SymbolType* type, Logger* logger, NodeExecutor* ne);
const Ast::Node* GetFirstNestedValue(const Ast::Node* designatorList, Logger *logger);
std::vector<ArrayInitPair> PartitionArrayInitializer(const Ast::Node *designatorList, const AccessArray* nextAcc, Logger* logger, NodeExecutor* nodeExec, PagedHeap* allocator);
bool IsPointer(const AccessArray *accArray, size_t startIdx = 0);
bool IsArray(const AccessArray *accArray);
