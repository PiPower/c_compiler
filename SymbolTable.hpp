#pragma once
#include <stdint.h>
#include <string_view>
#include <unordered_map>
#include "utils/FileManager.hpp"

namespace SymType
{

enum Kind: uint16_t
{
    NONE,
    TYPEDEF
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
    complex_long_double
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
    SymType::Kind kind;
};

struct SymbolTypedef
{
    SymType::Kind kind;
};

struct SymbolType
{
    SymType::Kind kind;
    BuiltIn::Type dType;
    TypeBits desc;
    // used only when type == struct_t
    size_t argCount;
    Argument* argsList;
};

struct SymbolTable
{
    SymbolTable();
    std::string_view AddSymbolName(const char* symName);
    void AddSymbol(const char* name, Symbol* sym);

    std::unordered_map<std::string_view, Symbol*> symbolTable;
    PagedBuffer symNameBuff;
};
