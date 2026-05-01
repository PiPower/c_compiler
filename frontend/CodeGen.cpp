#include "CodeGen.hpp"
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <sys/uio.h>
#include <unistd.h>
constexpr uint8_t TYPE_BUFFER = 0;
constexpr uint8_t FUNC_BUFFER = 1;

constexpr int FIRST_VALUE = -1;
constexpr int nr_of_pages = 7;
CodeGen::CodeGen(SymbolTable* symTab,  FileManager* manager, NodeExecutor* ne)
:
typeHeap(nr_of_pages), symTab(symTab), manager(manager), nodeExec(ne)
{
    for(size_t i =0 ; i < writableBufferArr.size(); i++)
    {
        currPtrArr[i] = typeHeap.allocateArray<char>(nr_of_pages * CPU_PAGE_SIZE);
        writableBufferArr[i].push_back(currPtrArr[i]);
    }
}

void CodeGen::EmitUnionStruct(SymbolType *symType, const std::string_view& name, bool flushQueue)
{
    if(symType->dType != BuiltIn::struct_t && 
       symType->dType != BuiltIn::union_t)
    {
        return;
    }

    auto emittedSym = emittedTypes.find(symType);
    if( emittedSym != emittedTypes.end() && emittedSym->second.isEmitted)
    {
        return;
    }

    EmitTypename(symType, name, false);
    WriteCharData(" = type { ");
    if(symType->dType == BuiltIn::struct_t)
    {
        for(size_t i =0; i < symType->argCount; i++)
        {
            EmitMember(&symType->memberList[i]);
            if( i < symType->argCount - 1)
            {
                WriteByteT(',');
                WriteByteT(' ');
            }
        }
    }
    else
    {
        size_t maxSize = 0;
        size_t maxAlignment = 1;
        size_t maxSizeMember = 0;
        size_t maxAlignmentMember = 0;

        for(size_t i =0; i < symType->argCount; i++)
        {
            if(symType->memberList[i].size > maxSize)
            {
                maxSize = symType->memberList[i].size;
                maxSizeMember = i;
            }

            if(symType->memberList[i].alignment > maxAlignment)
            {
                maxAlignment = symType->memberList[i].alignment;
                maxAlignmentMember = i;
            }
        }  
        if(maxAlignmentMember == maxSizeMember)
        {
            EmitMember(&symType->memberList[maxSizeMember]);
        }
        else
        {
            // enforces proper alignment 
            EmitMember(&symType->memberList[maxAlignmentMember]);
            size_t aling = maxSize % maxAlignment;
            maxSize += aling ? maxAlignment - aling : 0;
            maxSize -= symType->memberList[maxAlignmentMember].size;
            std::string num = std::to_string(maxSize);
            WriteCharData(", [%s x i8]", num.data(), num.length());
        }
    }
    WriteCharData(" }\n");
    // mark symbol as emitted
    emittedTypes[symType].isEmitted = true;

    if(flushQueue)
    {
        FlushTypeQueue();
    }
}

void CodeGen::EmitTypename(SymbolType *symType, const std::string_view& typeName, bool useQueue)
{
    if(symType->dType != BuiltIn::struct_t &&
       symType->dType != BuiltIn::union_t)
    {
        switch (symType->dType)
        {
        case BuiltIn::bool_t:    WriteCharData("i8");   break;
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

    // if anonynous struct emitt name as is
    if(typeName[0] == '%')
    {
        auto typeDesc = emittedTypes.find(symType);
        if(typeDesc == emittedTypes.end())
        {
            if(useQueue)
            {
                AddSymbolToEmitQueue(symType, typeName);
            }
            emittedTypes[symType] = {FIRST_VALUE, false};
            typeCounter[typeName] = {FIRST_VALUE};
        }
        WriteCharData("%s", typeName.data(), typeName.length());
        return;
    }
    else
    {
        auto typeDesc = emittedTypes.find(symType);
        if(typeDesc == emittedTypes.end())
        {
            auto typeCtr = typeCounter.find(typeName);
            if(typeCtr == typeCounter.end())
            {
                typeCounter[typeName] = FIRST_VALUE;
                emittedTypes[symType] = {FIRST_VALUE, false};
                typeDesc = emittedTypes.find(symType);
            }
            else
            {
                emittedTypes[symType] = {typeCtr->second, false};
                typeCtr->second++;
                typeDesc = emittedTypes.find(symType);
            }
            if(useQueue)
            {
                AddSymbolToEmitQueue(symType, typeName);
            }
        }

        if(typeDesc->second.symbolSaveCounter == FIRST_VALUE)
        {
            std::string_view inst = symType->dType ==  BuiltIn::struct_t ? "%%struct.%s" : "%%union.%s";
            WriteCharData(inst.data(), typeName.data(), typeName.length());
        }
        else
        {
            std::string_view inst = symType->dType ==  BuiltIn::struct_t ? "%%struct.%s.%d": "%%union.%s.%d";
            WriteCharData(inst.data(), typeName.data(), typeName.length(), typeDesc->second.symbolSaveCounter);
        }
    }   
    
}

void CodeGen::EmitMember(Member *member)
{
    static std::vector<std::string_view> arrSizes;

    if(member->access.type == ACC_NONE)
    {
        EmitTypename(symTab->QueryTypeSymbol(member->typeName), member->typeName);
        return;
    }

    AccessType* accType = &member->access;
    bool hitPointer = false;
    uint32_t brackets = 0;
    while (accType)
    {
        if(accType->type == ACC_POINTER)
        {
            hitPointer = true;
            break;
        }
        else if(accType->type == ACC_ARRAY)
        {
            if(!accType->array.asmExpr)
            {
                // variable length array
                WriteCharData("[0 x ");
            }
            else
            {
                Typed::Number num = nodeExec->ExecuteNode(accType->array.asmExpr);
                if(num.type == Typed::d_float || num.type == Typed::d_double)
                {
                    printf("Floats cannot be used inside array size declaration \n");
                    exit(-1);
                }
                std::string str = std::to_string(num.int64);
                WriteCharData("[%s x ", str.data(), str.length());
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
        WriteByteT(']');
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
            WriteByteT(curr);
            curr++;
            continue;
        }

        curr++;
        switch (*curr)
        {
        case '%':
            WriteByteT(curr);
            curr++;
            break;
        case 's':
        {
            curr++;
            const char* str = va_arg(args, const char*);
            size_t len = va_arg(args, size_t);
            while (len > 0)
            {
                WriteByteT(str);
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
                WriteByteT(num.data() + i);
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

void CodeGen::WriteByteT(const char* c)
{
    WriteByteImpl(TYPE_BUFFER, c);
}

void CodeGen::WriteByteT(char c)
{
    WriteByteImpl(TYPE_BUFFER, c);
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

void CodeGen::AddSymbolToEmitQueue(SymbolType *symType, const std::string_view &name)
{
    typeQueue.push({symType, name});
}

void CodeGen::FlushTypeQueue()
{
    while (typeQueue.size() > 0)
    {
        PendingType pt = typeQueue.front();
        typeQueue.pop();
        EmitUnionStruct(pt.symType, pt.name, false);
    }
    
}

void CodeGen::WriteToFile(int fd)
{
    constexpr std::array<uint8_t, 2> buffOrder = {TYPE_BUFFER, FUNC_BUFFER};
    size_t maxSize = 0;
    for(const auto& arr : writableBufferArr)
    {
        maxSize = std::max(arr.size(), maxSize);
    }
    iovec *iov = new iovec[maxSize];
    for(uint8_t buffIdx : buffOrder)
    {
        const std::vector<char*>& buff = writableBufferArr[buffIdx];
        if(buff.size() == 1 && 
           currPtrArr[buffIdx] - buff[0] == 0)
        {
            continue;
        }

        size_t i=0;
        for(; i < buff.size() - 1; i++)
        {
            iov[i].iov_base =  buff[i];
            iov[i].iov_len = typeHeap.GetAllocSize();
        }
        iov[i].iov_base =  buff[i];
        iov[i].iov_len = currPtrArr[buffIdx] - buff[i];

        writev(fd, iov, (int)buff.size());
        fsync(fd);
    }

    delete[] iov;
}

void CodeGen::WriteByteImpl(uint8_t bufferType, const char *c)
{
    WriteByteImpl(bufferType, *c);
}

void CodeGen::WriteByteImpl(uint8_t bufferType, char c)
{
    std::vector<char*>* buffs = &writableBufferArr[bufferType];
    char* currPtr = currPtrArr[bufferType];
    if((uint64_t)(currPtr - buffs->back()) == typeHeap.GetAllocSize())
    {
        currPtrArr[bufferType] = typeHeap.allocateArray<char>(nr_of_pages * CPU_PAGE_SIZE);
        currPtr = currPtrArr[bufferType];
        buffs->push_back(currPtrArr[bufferType]);
    }

    *currPtr = c;
    currPtrArr[bufferType]++;
}
