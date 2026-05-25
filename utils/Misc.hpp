#pragma once
#include "../LangTypes.hpp"
#include "../frontend/NodeExecutor.hpp"
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

ArraySize GetArrayElemCount(AccessArray *accArray, Logger* logger, NodeExecutor* ne);
MemoryDesc GetMemoryDesc(AccessArray *accArray, SymbolType* type, Logger* logger, NodeExecutor* ne);

bool IsPointer(const AccessArray *accArray, size_t startIdx = 0);
bool IsArray(const AccessArray *accArray);
AccessType* MargeAccessTypes(const AccessType* first, const AccessType* second);
void FreeMergedAccType(AccessType* acc);