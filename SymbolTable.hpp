#pragma once
#include <stdint.h>
#include <string_view>
#include <unordered_map>
#include "utils/FileManager.hpp"
#include "frontend/AstNode.hpp"
#include <deque>
#include <array>
#include <cstddef>

constexpr uint8_t NONE = 0;
constexpr uint8_t POINTER = 1;
constexpr uint8_t FN_DECL = 2;
constexpr uint8_t FN_CALL = 3;
constexpr uint8_t ARRAY = 4;

namespace Sym
{

enum Kind: uint16_t
{
    NONE = 0,
    TYPEDEF = 0x1 << 0,
    TYPE = 0x1 << 1
};

}

namespace BuiltIn
{

enum Type : uint16_t
{
    none,
    struct_t,
    union_t,
    enum_t,
    ptr,
    s_char_8,         
    u_char_8,         
    s_int_16,         
    u_int_16,         
    s_int_32,         
    u_int_32,         
    s_int_64,        
    u_int_64,        
    float_32,         
    double_64,        
    long_double,      
    void_t,           
    bool_t,           
    complex_float_64, 
    complex_double_128,
    complex_long_double,
    special
};

}

namespace Scope
{

enum Type : uint8_t 
{
    NONE,
    GLOBAL,
    LOCAL,
    STRUCT
};

}

struct Qualifiers
{
    uint8_t const_ : 1;
    uint8_t restrict_ : 1;
    uint8_t volatile_ : 1;
};

struct Spec
{
    uint8_t typedef_ : 1;
    uint8_t extern_ : 1;
    uint8_t static_ : 1;
    uint8_t auto_ : 1;
    uint8_t register_ : 1;
};

struct Pointer
{
    Qualifiers quals;
};

struct FnDecl
{
    Ast::Node* paramTypeList;
};

struct FnCall
{
    Ast::Node* identifierList;
};

struct Array
{
    Ast::Node* asmExpr;
    Ast::Node* qualList;
};



struct AccessType
{
    uint8_t type;
    union 
    {
        Pointer ptr;
        FnDecl fnDecl;
        FnCall fnCall;
        Array array;
    };
    uint32_t level;
    AccessType* next;
};

struct TypeBits
{
    // qualifiers
    union
    {
        Qualifiers qual;
        uint8_t qualifierFlags;
    };
    // storage class specs
    union
    {
        Spec spec;
        uint8_t storageFlags;
    };
    // function specs
    uint8_t inlineSpec : 1;
};


struct Member
{
    AccessType access;
    TypeBits declType; 
    BuiltIn::Type memberType;
    int64_t bitCount;
    std::string_view typeName;
};


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
    // used only when type == struct_t or union
    size_t argCount;
    // points to table that holds symbol in struct's scope
    ScopedSymbolTable* structTable;
    std::string_view* memberNames;
    Member* memberList;
    bool isDefined;
};

struct ScopedSymbolTable
{
    ScopedSymbolTable* parent;
    std::array<std::unordered_map<std::string_view, Symbol*>, 4> tables;
    uint8_t scopeType;
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
    Sym::Kind symKind;
    if constexpr (std::is_same_v<Kind, SymbolType>){symKind = Sym::TYPE;}
    else if constexpr (std::is_same_v<Kind, SymbolTypedef>){symKind = Sym::TYPEDEF;}
    Kind* symbol = new Kind(symKind, std::forward<const Args>(args)...);
    AddSymbolImpl(name, (Symbol*) symbol);
}
