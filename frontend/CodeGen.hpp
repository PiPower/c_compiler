#pragma once
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../SymbolTable.hpp"
#include "../utils/PagedHeap.hpp"
#include "../utils/FileManager.hpp"
constexpr uint8_t LlvmTypeStruct = 0;
constexpr uint8_t LlvmTypeUnion = 1;

struct LlvmType
{
    int symbolSaveCounter;
};


struct CodeGen
{
    CodeGen(SymbolTable* symTab,  FileManager* manager);
    void EmitUnionStruct(SymbolType* symType, const std::string_view& name);
    void EmitTypename(SymbolType* symType, const std::string_view& typeName);
    void EmitMember(Member* member);
    void WriteCharData(const char* data, ...);
    void inline WriteByte(const char* c);
    void inline WriteByte(char c);
    std::string_view GetViewForToken(const Token &token);
    void WriteToFile(int fd);

    std::unordered_map<std::string_view, int> typeCounter;
    std::unordered_map<SymbolType*, LlvmType> emittedTypes;

    uint64_t remainingMemory;
    char* bufferData;
    PagedHeap typeHeap;
    SymbolTable* symTab;
    FileManager* manager;
};
