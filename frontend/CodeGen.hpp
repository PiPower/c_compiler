#pragma once
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../SymbolTable.hpp"
#include "../utils/PagedHeap.hpp"
#include "../utils/FileManager.hpp"
#include "NodeExecutor.hpp"
constexpr uint8_t LlvmTypeStruct = 0;
constexpr uint8_t LlvmTypeUnion = 1;

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

struct CodeGen
{
    CodeGen(SymbolTable* symTab,  FileManager* manager, NodeExecutor* ne);
    void EmitUnionStruct(SymbolType* symType, const std::string_view& name, bool flushQueue = true);
    void EmitTypename(SymbolType* symType, const std::string_view& typeName, bool useQueue = true);
    void EmitMember(Member* member);
    void WriteCharData(const char* data, ...);
    std::string_view GetViewForToken(const Token &token);
    void AddSymbolToEmitQueue(SymbolType* symType, const std::string_view& name);
    void FlushTypeQueue();
    void WriteToFile(int fd);
    void WriteByteT(const char* c);
    void WriteByteT(char c);
    void WriteByteImpl(uint8_t bufferType, const char* c);
    void WriteByteImpl(uint8_t bufferType, char c);

    std::unordered_map<std::string_view, int> typeCounter;
    std::unordered_map<SymbolType*, LlvmType> emittedTypes;
    std::queue<PendingType> typeQueue;

    std::array<std::vector<char*>, 2> writableBufferArr;
    std::array<char*, 2> currPtrArr;
    PagedHeap typeHeap;
    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor* nodeExec;
};
