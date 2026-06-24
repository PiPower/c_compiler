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

Ast::Node* BuildInitializerList(const std::vector<Ast::Node>& initItems, PagedHeap* allocator);
ArraySize GetArrayElemCount(const AccessArray *accArray, Logger* logger, NodeExecutor* ne);
MemoryDesc GetMemoryDesc(const AccessArray *accArray, SymbolType* type, Logger* logger, NodeExecutor* ne);
const Ast::Node* GetFirstNestedValue(const Ast::Node* designatorList, Logger *logger);
std::vector<ArrayInitPair> PartitionArrayInitializer(const Ast::Node *designatorList, const AccessArray* nextAcc, Logger* logger, NodeExecutor* nodeExec, PagedHeap* allocator);
bool IsPointer(const AccessArray *accArray, size_t startIdx = 0);
bool DecaysToPointer(const AccessArray *accArray, size_t startIdx = 0);
bool IsArray(const AccessArray *accArray);
std::string_view GetBuiltInName(const BuiltIn::Type type);
BuiltIn::Type GetBuiltInType(const std::string_view& typeName);
uint32_t GetBuiltInAlignemnt(const BuiltIn::Type type);
std::string_view GetViewForToken(const Token &token, FileManager* fm);
Ast::NodeType tokenAsmToExpr(TokenType::Type asmToken);

constexpr inline Ast::Node* lenToAstPtr(size_t len)
{
    return reinterpret_cast<Ast::Node*>(static_cast<uintptr_t>(len));
}

constexpr inline size_t astPtrToLen(Ast::Node* node)
{
    return static_cast<size_t>(reinterpret_cast<uintptr_t>(node));
}

constexpr inline bool nodeIsNotCompoundLiteral(const Ast::Node* root)
{
    return !(root->type == Ast::cast || 
            (root->type == Ast::get_addr && root->lChild && root->lChild->type == Ast::cast));
}

constexpr inline bool isSmallInteger(BuiltIn::Type type)
{
    return type >=  BuiltIn::s_char_8 && type <=  BuiltIn::u_int_16;
}

constexpr inline bool isInteger(BuiltIn::Type type)
{
    return type >=  BuiltIn::s_char_8 && type <=  BuiltIn::u_int_64;
}

constexpr inline bool isFloat(BuiltIn::Type type)
{
    return type >=  BuiltIn::float_32 && type <=  BuiltIn::long_double;
}

constexpr inline bool isUnsigned(BuiltIn::Type type)
{
    return type == BuiltIn::u_char_8 || type == BuiltIn::u_int_16 ||
           type == BuiltIn::u_int_32|| type == BuiltIn::u_int_64;
}

constexpr inline bool isSigned(BuiltIn::Type type)
{
    return type == BuiltIn::s_char_8 || type == BuiltIn::s_int_16 ||
           type == BuiltIn::s_int_32|| type == BuiltIn::s_int_64;
}

constexpr inline bool isStructOrUnion(BuiltIn::Type type)
{
    return type == BuiltIn::struct_t || type == BuiltIn::union_t;
}

inline Typed::Number CastToBuiltIn(BuiltIn::Type type, const Typed::Number& num)
{
    Typed::Number out{};
    switch (type)
    {
    case BuiltIn::Type::s_char_8:
        out.int8 = CastTo<int8_t>(num);
        out.type = Typed::DType::d_int8_t;
        return out;
    case BuiltIn::Type::u_char_8:
        out.uint8 = CastTo<uint8_t>(num);
        out.type = Typed::DType::d_uint8_t;
        return out;
    case BuiltIn::Type::s_int_16:
        out.int16 = CastTo<int16_t>(num);
        out.type = Typed::DType::d_int16_t;
        return out;
    case BuiltIn::Type::u_int_16:
        out.uint16 = CastTo<uint16_t>(num);
        out.type = Typed::DType::d_uint16_t;
        return out;
    case BuiltIn::Type::s_int_32:
        out.int32 = CastTo<int32_t>(num);
        out.type = Typed::DType::d_int32_t;
        return out;
    case BuiltIn::Type::u_int_32:
        out.uint32 = CastTo<uint32_t>(num);
        out.type = Typed::DType::d_uint32_t;
        return out;
    case BuiltIn::Type::s_int_64:
        out.int64 = CastTo<int64_t>(num);
        out.type = Typed::DType::d_int64_t;
        return out;
    case BuiltIn::Type::u_int_64:
        out.uint64 = CastTo<uint64_t>(num);
        out.type = Typed::DType::d_uint64_t;
        return out;
    case BuiltIn::Type::float_32:
        out.float32 = CastTo<float>(num);
        out.type = Typed::DType::d_float;
        return out;
    case BuiltIn::Type::double_64:
        out.float64 = CastTo<double>(num);
        out.type = Typed::DType::d_double;
        return out;
    case BuiltIn::Type::long_double:
        out.float64 = CastTo<long double>(num);
        out.type = Typed::DType::d_l_double;
        return out;
    default:
        return {0, Typed::DType::d_none};
    }
}
