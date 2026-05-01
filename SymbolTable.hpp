#pragma once
#include <stdint.h>
#include <string_view>
#include <unordered_map>
#include "utils/FileManager.hpp"
#include "LangTypes.hpp"
#include <deque>
#include <array>
#include <cstddef>

struct Symbol
{
    Sym::Kind kind;
};

struct SymbolTypedef
{
    Sym::Kind kind;
    std::string_view refrencedType;
    Qualifiers qual;
};

struct ScopedSymbolTable;
struct SymbolType
{
    Sym::Kind kind;
    BuiltIn::Type dType;
    bool isDefined;
    uint64_t size;
    uint32_t alignment;
    // used only when type == struct_t or union
    size_t argCount;
    // points to table that holds symbol in struct's scope
    ScopedSymbolTable* structTable;
    std::string_view* memberNames;
    Member* memberList;
};

struct SymbolFunction
{
    Sym::Kind kind;
    ScopedSymbolTable* fnScope;
    bool isDefined;
};

struct ScopedSymbolTable
{
    ScopedSymbolTable* parent;
    std::array<std::unordered_map<std::string_view, Symbol*>, 4> tables;
    uint8_t scopeType;
};

template <typename T>
struct SymbolKindTraits;

template <>
struct SymbolKindTraits<SymbolType> {
    static constexpr Sym::Kind value = Sym::TYPE;
};

template <>
struct SymbolKindTraits<SymbolTypedef> {
    static constexpr Sym::Kind value = Sym::TYPEDEF;
};

template <>
struct SymbolKindTraits<SymbolFunction> {
    static constexpr Sym::Kind value = Sym::FUNC;
};

struct SymbolTable
{
  
    SymbolTable();
    std::string_view AddSymbolName(const char* symName);
    void AddSymbolImpl(const std::string_view&, Symbol* sym);
    void CreateNewScope(Scope::Type scopeType);
    void PopScope();
    char* HeapAllocateAligned(uint64_t size, uint8_t alignment);
    uint16_t QuerySymKinds(const std::string_view& name);
    SymbolType* QueryTypeSymbol(
        const std::string_view& name,
        uint8_t* scopeType = nullptr,
        uint8_t* prevScope = nullptr);
    SymbolTypedef* QueryTypedefSymbol(
        const std::string_view& name,
        uint8_t* scopeType = nullptr,
        uint8_t* prevScope = nullptr);
    Symbol* QuerySymbolGeneric(
        const std::string_view& name, 
        uint8_t tableIdx,
        uint8_t* scopeType = nullptr,
        uint8_t* prevScope = nullptr);

    template<typename Type>
    Type* AllocateTypeOnHeap();
    template<typename Type>
    Type* AllocateTypeArrayOnHeap(uint64_t count);
    template<typename Kind, typename... Args>
    void AddSymbol(const std::string_view&, Args&&... args);

    ScopedSymbolTable* globalTable;
    ScopedSymbolTable* currentTable;
    std::deque<ScopedSymbolTable> tableBufferHandle;
    PagedBuffer symNameBuff;
    // heap to be used by anything that has to do with data inside symbol table
    PagedBuffer symbolHeap;
};

template <typename Type>
inline Type* SymbolTable::AllocateTypeOnHeap()
{
    char* data = HeapAllocateAligned(sizeof(Type), alignof(std::max_align_t));
    if(!data) return nullptr;
    return new (data)Type;
}

template <typename Type>
inline Type *SymbolTable::AllocateTypeArrayOnHeap(uint64_t count)
{
    char* data = HeapAllocateAligned(count * sizeof(Type), alignof(std::max_align_t));
    if(!data) return nullptr;
    return new (data)Type[count];
}

template <typename Kind, typename... Args>
void SymbolTable::AddSymbol(const std::string_view& name, Args &&...args)
{
    Sym::Kind symKind = SymbolKindTraits<Kind>::value;
    Kind* symbol = new Kind(symKind, std::forward<const Args>(args)...);
    AddSymbolImpl(name, (Symbol*) symbol);
}
