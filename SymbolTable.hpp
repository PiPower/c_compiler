#pragma once
#include <stdint.h>
#include <string_view>
#include <unordered_map>
#include "utils/FileManager.hpp"
#include <deque>
namespace Sym
{

enum Kind: uint16_t
{
    NONE,
    TYPEDEF,
    TYPE
};

}

namespace BuiltIn
{

enum Type : uint16_t {
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


struct Argument
{
    BuiltIn::Type* kind;
    std::string_view structName;
};


struct Symbol
{
    Sym::Kind kind;
};

struct SymbolTypedef
{
    Sym::Kind kind;
    std::string_view refrencedType;
};

struct SymbolType
{
    Sym::Kind kind;
    BuiltIn::Type dType;
    TypeBits desc;
    // used only when type == struct_t
    size_t argCount;
    Argument* argsList;
};

struct ScopedSymbolTable
{
    ScopedSymbolTable* parent;
    std::unordered_map<std::string_view, Symbol*> table;
};


struct SymbolTable
{
  
    SymbolTable();
    std::string_view AddSymbolName(const char* symName);
    void AddSymbolImpl(std::string_view name, Symbol* sym);
    Sym::Kind QuerySymKind(const std::string_view& name);

    template<typename Kind, typename... Args>
    void AddSymbol(std::string_view name, Args&&... args);

    ScopedSymbolTable* globalTable;
    ScopedSymbolTable* currentTable;
    std::deque<ScopedSymbolTable> tableBufferHandle;
    PagedBuffer symNameBuff;
};

template<typename Kind, typename... Args>
void SymbolTable::AddSymbol(std::string_view name, Args&&... args)
{
    Sym::Kind symKind;
    if constexpr (std::is_same_v<Kind, SymbolType>){symKind = Sym::TYPE;}
    else if constexpr (std::is_same_v<Kind, SymbolTypedef>){symKind = Sym::TYPEDEF;}
    Kind* symbol = new Kind(symKind, std::forward<const Args>(args)...);
    AddSymbolImpl(name, (Symbol*) symbol);
}
