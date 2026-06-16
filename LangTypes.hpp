#pragma once
#include <stdint.h>
#include <string_view>
#include "frontend/AstNode.hpp"

struct DeclSpecs;
struct Declarator;

constexpr uint8_t ACC_NONE = 0;
constexpr uint8_t ACC_POINTER = 1;
constexpr uint8_t ACC_FN_DECL = 1 << 1;
constexpr uint8_t ACC_FN_CALL = 1 << 2;
constexpr uint8_t ACC_ARRAY = 1 << 3;
constexpr uint8_t ACC_ARRAY_VLA = 1 << 4;

namespace Sym
{

enum Kind: uint16_t
{
    NONE = 0,
    TYPEDEF = 0x1 << 0,
    TYPE = 0x1 << 1,
    FUNC = 0x1 << 2,
    VAR = 0x1 << 3
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
    string,
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
    compound_literal,
    special,
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

inline bool operator!=(const Qualifiers& lhs, const Qualifiers& rhs)
{
    return lhs.const_ != rhs.const_ || lhs.restrict_ != rhs.restrict_ || lhs.volatile_ != rhs.volatile_ ;
}

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

struct FunctionParams;
struct FnDecl
{
    size_t paramCount;
    FunctionParams* paramTypeList;
};

struct FnCall
{
    Ast::Node* identifierList;
};

struct Array
{
    union
    {    
        Ast::Node* asmExpr; // if VLA
        uint64_t size; // if non-VLA
    };
    Qualifiers quals;
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

struct DeclaratorAcces
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
    uint8_t isEllipsis : 1;
};

struct AccessArray
{
    AccessType* ptr;
    size_t count;
};

struct Declarator
{
    Token token;
    AccessArray accArr;
    std::string_view name; // abstract declarator has empty name
    const Ast::Node* initExpr;
};


struct Member
{
    AccessArray accArr;
    AccessArray typedefAccArr;
    TypeBits declType; 
    BuiltIn::Type memberType;
    int64_t bitCount;
    uint64_t size;
    std::string_view typeName;
    uint32_t alignment;
};

struct SymbolType;
struct DeclSpecs
{
    TypeBits declType; 
    std::string_view typenameView; 
    SymbolType* symType;
    const AccessArray* accArr;
};

struct FunctionParams
{
    DeclSpecs spec;
    Declarator decl;
};

struct InitDeclarator
{
    Declarator decl;
    const Ast::Node* initializer;
};

struct StructDeclarator
{
    Declarator decl;
    int64_t bitCount;
};

struct StructDeclaration
{
    DeclSpecs declSpec;
    std::vector<StructDeclarator> declarators;
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
    AccessArray accArr;
};

struct ScopedSymbolTable;
struct StructDesc
{
    // name that is assigned to struct by codegen
    int64_t codeGenIdx;
    // used only when type == struct_t or union
    size_t argCount;
    // points to table that holds symbol in struct's scope
    ScopedSymbolTable* structTable;
    std::string_view* memberNames;
    Member* memberList;
};

struct PointerDesc
{
    AccessArray accessTypes;
    DeclSpecs spec;
};

struct SymbolType
{
    Sym::Kind kind;
    BuiltIn::Type dType;
    uint8_t isDefined : 1;
    uint8_t passByValue : 1; // if value is larger than 16 bytes or unaligned pass by stack else by value
    uint64_t size;
    uint32_t alignment;
    union 
    {
        StructDesc str;
        PointerDesc ptr;
    };
        // Constructor for StructDesc
    SymbolType(Sym::Kind k,
               BuiltIn::Type dt,
               bool def,
               bool passByValue,
               uint64_t s,
               uint32_t align,
               const StructDesc& sd)
        : kind(k), dType(dt), isDefined(def), passByValue(passByValue), size(s), alignment(align), str(sd)
        {}

    // Constructor for PointerDesc
    SymbolType(Sym::Kind k,
               BuiltIn::Type dt,
               bool def,
               bool passByValue,
               uint64_t s,
               uint32_t align,
               const PointerDesc& pd)
        : kind(k), dType(dt), isDefined(def), passByValue(passByValue), size(s), alignment(align), ptr(pd)
        {}


};

struct SymbolFunction
{
    Sym::Kind kind;
    DeclSpecs spec;
    Declarator decl;
    uint32_t paramCount;
    uint32_t retPtrOrder; // if pointer is to be returned, gives pointer order
    FunctionParams* params;
    ScopedSymbolTable* fnScope;
    bool isDefined;
};

struct VariableOpts
{
    uint8_t isEnumerator : 1;
    uint8_t isConst : 1;
    uint8_t isEmitted : 1;
};
struct SymbolVariable
{
    Sym::Kind kind;
    Scope::Type scope;
    DeclSpecs spec;
    Declarator decl;
    VariableOpts opts;
    // if idx == -1 unused
    // if isEnumerator == 1 then it represents value of that enumerator
    // otherwise its index in codegen in local scope
    int64_t varIdx;

    SymbolVariable(
        Sym::Kind kind,  
        Scope::Type scope, 
        const DeclSpecs* spec,
        const Declarator* decl, 
        const VariableOpts* opts,
        int64_t idx) 
    :
    kind(kind), scope(scope), spec(*spec), decl(*decl), opts(*opts), varIdx(idx) {}
};

struct ScopedSymbolTable
{
    ScopedSymbolTable* parent;
    int64_t id;
    std::array<std::unordered_map<std::string_view, Symbol*>, 4> tables;
    Scope::Type scopeType;
};