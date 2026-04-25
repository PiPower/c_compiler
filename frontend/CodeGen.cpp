#include "CodeGen.hpp"
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <sys/uio.h>
#include <unistd.h>

constexpr int FIRST_VALUE = -1;
constexpr int nr_of_pages = 7;
CodeGen::CodeGen(SymbolTable* symTab,  FileManager* manager)
:
typeHeap(nr_of_pages), symTab(symTab), manager(manager)
{
    bufferData = typeHeap.allocateArray<char>(nr_of_pages * CPU_PAGE_SIZE);
    remainingMemory = typeHeap.GetAllocSize();
}

void CodeGen::EmitUnionStruct(SymbolType *symType, const std::string_view& name)
{
    if(symType->dType != BuiltIn::struct_t && 
       symType->dType != BuiltIn::union_t)
    {
        return;
    }

    if(emittedTypes.find(symType) != emittedTypes.end())
    {
        return;
    }

    EmitTypename(symType, name);
    WriteCharData(" = type { ");
    for(size_t i =0; i < symType->argCount; i++)
    {
        EmitMember(&symType->memberList[i]);
        if( i < symType->argCount - 1)
        {
            WriteByte(',');
            WriteByte(' ');
        }
        Member& currMember = symType->memberList[i];
    }
    WriteCharData(" }\n");
}

void CodeGen::EmitTypename(SymbolType *symType, const std::string_view& typeName)
{
    if(symType->dType != BuiltIn::struct_t &&
       symType->dType != BuiltIn::union_t)
    {
        switch (symType->dType)
        {
        case BuiltIn::s_char_8:  WriteCharData("i8");   break;
        case BuiltIn::u_char_8:  WriteCharData("i8");   break;
        case BuiltIn::s_int_16:  WriteCharData("i16");  break;
        case BuiltIn::u_int_16:  WriteCharData("i16");  break;
        case BuiltIn::s_int_32:  WriteCharData("i32");  break;
        case BuiltIn::u_int_32:  WriteCharData("i32");  break;
        case BuiltIn::s_int_64:  WriteCharData("i64");  break;
        case BuiltIn::u_int_64:  WriteCharData("i64");  break;
        case BuiltIn::float_32:  WriteCharData("float");   break;
        case BuiltIn::double_64: WriteCharData("double");  break;
        case BuiltIn::ptr: WriteCharData("ptr");  break;
        default:
            printf("code gen: type unsupported");
            exit(-1);
            break;
        }

        return;
    }

    auto typeDesc = emittedTypes.find(symType);
    if(typeDesc == emittedTypes.end())
    {
        auto typeCtr = typeCounter.find(typeName);
        if(typeCtr == typeCounter.end())
        {
            typeCounter[typeName] = FIRST_VALUE;
            emittedTypes[symType] = {FIRST_VALUE};
            typeDesc = emittedTypes.find(symType);
        }
        else
        {
            emittedTypes[symType] = {typeCtr->second};
            typeCtr->second++;
            typeDesc = emittedTypes.find(symType);
        }
    }

    // if anonynous struct emitt name as is
    if(typeName[0] == '%')
    {
        WriteCharData("%s", typeName.data(), typeName.length());
        return;
    }
    else if(symType->dType == BuiltIn::struct_t)
    {
        if(typeDesc->second.symbolSaveCounter == FIRST_VALUE)
        {
            WriteCharData("%%struct.%s", typeName.data(), typeName.length());
        }
        else
        {
            WriteCharData("%%struct.%s.%d", typeName.data(), typeName.length(), typeDesc->second.symbolSaveCounter);
        }
    }
    else
    {
        if(typeDesc->second.symbolSaveCounter == FIRST_VALUE)
        {
            WriteCharData("%%union.%s", typeName.data(), typeName.length());
        }
        else
        {
            WriteCharData("%%union.%s.%d", typeName.data(), typeName.length(), typeDesc->second.symbolSaveCounter);
        }
    }
    
}

void CodeGen::EmitMember(Member *member)
{
    static std::vector<std::string_view> arrSizes;

    if(member->access.type == NONE)
    {
        EmitTypename(symTab->QueryTypeSymbol(member->typeName), member->typeName);
        return;
    }

    AccessType* accType = &member->access;
    bool hitPointer = false;
    uint32_t brackets = 0;
    while (accType)
    {
        if(accType->type == POINTER)
        {
            hitPointer = true;
            break;
        }
        else if(accType->type == ARRAY)
        {
            if(!accType->array.asmExpr)
            {
                // variable length array
                WriteCharData("[0 x ");
            }
            else
            {
                if(accType->array.asmExpr->token.type != TokenType::numeric_constant)
                {
                    printf("Non constants are not supported currently \n");
                    exit(-1);
                }
                std::string_view view = GetViewForToken(accType->array.asmExpr->token);
                WriteCharData("[%s x ", view.data(), view.length());
            }
            brackets++;
        }
        else
        {
            printf("calls are not allowed in declarator \n");
            exit(-1);
        }
        accType = accType->next;
    }
    
    if(hitPointer)
    {
        WriteCharData("ptr");
    }
    else
    {
        EmitTypename(symTab->QueryTypeSymbol(member->typeName), member->typeName);
    }

    for(uint32_t i =0; i < brackets; i++)
    {
        WriteByte(']');
    }
}

void CodeGen::WriteCharData(const char *data, ...)
{
    const char* curr = data;
    va_list args;
    va_start(args, data);

    while (*curr != '\0')
    {
        if(*curr != '%')
        {
            WriteByte(curr);
            curr++;
            continue;
        }

        curr++;
        switch (*curr)
        {
        case '%':
            WriteByte(curr);
            curr++;
            break;
        case 's':
        {
            curr++;
            const char* str = va_arg(args, const char*);
            size_t len = va_arg(args, size_t);
            while (len > 0)
            {
                WriteByte(str);
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
                WriteByte(num.data() + i);
                i++;
            }
        }break;
        default:
            printf("Unsuported character in WriteCharData\n");
            exit(-1);
            break;
        }

    }
    va_end(args);
}

inline void CodeGen::WriteByte(const char* c)
{
    if(remainingMemory == 0)
    {
        bufferData = typeHeap.allocateArray<char>(nr_of_pages * CPU_PAGE_SIZE);
        remainingMemory = typeHeap.GetAllocSize();
    }

    *bufferData = *c;
    bufferData++;
    remainingMemory--;
}

inline void CodeGen::WriteByte(char c)
{
    if(remainingMemory == 0)
    {
        bufferData = typeHeap.allocateArray<char>(nr_of_pages * CPU_PAGE_SIZE);
        remainingMemory = typeHeap.GetAllocSize();
    }

    *bufferData = c;
    bufferData++;
    remainingMemory--;
}

std::string_view CodeGen::GetViewForToken(const Token &token)
{
    FILE_STATE state;
    if(manager->GetFileState(&token.location.id, &state) != 0)
    {
        printf("File Manager error: Requested file does not exit\n");
        exit(-1);
    }

    // removes \" from both start and end 
    uint8_t offset = token.type == TokenType::string_literal ? 1 : 0;
    std::string_view tokenView(state.fileData + token.location.offset + offset,
                                token.location.len - offset);
    return tokenView;
}

void CodeGen::WriteToFile(int fd)
{
    iovec *iov = (iovec *)alloca(sizeof(iovec) * typeHeap.basePtrs.size());
    size_t i=0;
    for(; i < typeHeap.basePtrs.size() - 1; i++)
    {
        iov[i].iov_base =  typeHeap.basePtrs[i];
        iov[i].iov_len = typeHeap.GetAllocSize();
    }
    iov[i].iov_base =  typeHeap.basePtrs[i];
    iov[i].iov_len = bufferData - (char*)typeHeap.basePtrs.back();

    writev(fd, iov, (int)typeHeap.basePtrs.size());
    fsync(fd);
}
