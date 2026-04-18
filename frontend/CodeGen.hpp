#pragma once
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../SymbolTable.hpp"
#include "../utils/PagedHeap.hpp"
constexpr uint8_t LlvmTypeStruct = 0;
constexpr uint8_t LlvmTypeUnion = 1;

struct LlvmType
{
    int symbolSaveCounter;
};


struct CodeGen
{
    CodeGen(SymbolTable* symTab);
    void EmitUnionStruct(SymbolType* symType);
    void EmitMember(Member* member);
    void WriteCharData(const char* data);

    std::unordered_map<std::string_view, int> typeCounter;
    std::unordered_map<SymbolType*, LlvmType> emittedTypes;

    uint64_t remainingMemory;
    PagedHeap typeHeap;
    SymbolTable* symTab;
};
