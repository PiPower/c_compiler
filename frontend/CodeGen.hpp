#pragma once
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../SymbolTable.hpp"
#include "../utils/PagedHeap.hpp"
#include "../utils/FileManager.hpp"
#include "NodeExecutor.hpp"
#include <stdarg.h>


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
    void EmitGlobalVariable(const DeclSpecs* spec, const Declarator* decl);
    void EmitLocalVariable(const DeclSpecs* spec, const Declarator* decl);
    std::string_view GetViewForToken(const Token &token);
    void AddSymbolToEmitQueue(SymbolType* symType, const std::string_view& name);
    void FlushTypeQueue();
    void WriteToFile(int fd);
    void WriteByteT(const char* c);
    void WriteByteT(char c);
    void WriteByteGV(const char* c);
    void WriteByteGV(char c);
    void WriteByteLV(const char* c);
    void WriteByteLV(char c);
    void WriteByteImpl(uint8_t bufferType, const char* c);
    void WriteByteImpl(uint8_t bufferType, char c);
    void WriteCharDataT(const char* data, ...);

    template<uint8_t BufferType>
    void WriteCharData(const char* data, va_list args);

    std::unordered_map<std::string_view, int> typeCounter;
    std::unordered_map<SymbolType*, LlvmType> emittedTypes;
    std::queue<PendingType> typeQueue;

    std::array<std::vector<char*>, 4> writableBufferArr;
    std::array<char*, 4> currPtrArr;
    PagedHeap typeHeap;
    SymbolTable* symTab;
    FileManager* manager;
    NodeExecutor* nodeExec;
    Logger logger;
};


template<uint8_t BufferType>
void CodeGen::WriteCharData(const char* data, va_list args)
{
    const char* curr = data;

    while (*curr != '\0')
    {
        if(*curr != '%')
        {
            WriteByteImpl(BufferType, curr);
            curr++;
            continue;
        }

        curr++;
        switch (*curr)
        {
        case '%':
            WriteByteImpl(BufferType, curr);
            curr++;
            break;
        case 's':
        {
            curr++;
            const char* str = va_arg(args, const char*);
            size_t len = va_arg(args, size_t);
            while (len > 0)
            {
                WriteByteImpl(BufferType, str);
                str++;
                len--;
            }
        }break;
        case 'd':
        {
            curr++;
            int c = va_arg(args, int);
            std::string num = std::to_string(c);
            size_t i = 0;
            while (num[i] != '\0')
            {
                WriteByteImpl(BufferType, num.data() + i);
                i++;
            }
        }break;
        default:
            printf("Unsuported character in WriteCharData\n");
            exit(-1);
            break;
        }

    }
}
