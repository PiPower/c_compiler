#include "CodeGen.hpp"
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <sys/uio.h>
#include <unistd.h>
#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl("Code Gen", tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

constexpr uint8_t TYPE_BUFFER = 0;
constexpr uint8_t FUNC_BUFFER = 1;
constexpr uint8_t GLOB_VAR_BUFFER = 1;
constexpr uint8_t LOC_VAR_BUFFER = 1;

constexpr int FIRST_VALUE = -1;
constexpr int nr_of_pages = 7;

CodeGen::CodeGen(SymbolTable* symTab,  FileManager* manager, NodeExecutor* ne)
:
typeHeap(nr_of_pages), symTab(symTab), manager(manager), nodeExec(ne), logger(manager)
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
    WriteCharDataT(" = type { ");
    if(symType->dType == BuiltIn::struct_t)
    {
        for(size_t i =0; i < symType->str.argCount; i++)
        {
            EmitMember(&symType->str.memberList[i]);
            if( i < symType->str.argCount - 1)
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

        for(size_t i =0; i < symType->str.argCount; i++)
        {
            if(symType->str.memberList[i].size > maxSize)
            {
                maxSize = symType->str.memberList[i].size;
                maxSizeMember = i;
            }

            if(symType->str.memberList[i].alignment > maxAlignment)
            {
                maxAlignment = symType->str.memberList[i].alignment;
                maxAlignmentMember = i;
            }
        }  
        if(maxAlignmentMember == maxSizeMember)
        {
            EmitMember(&symType->str.memberList[maxSizeMember]);
        }
        else
        {
            // enforces proper alignment 
            EmitMember(&symType->str.memberList[maxAlignmentMember]);
            size_t aling = maxSize % maxAlignment;
            maxSize += aling ? maxAlignment - aling : 0;
            maxSize -= symType->str.memberList[maxAlignmentMember].size;
            std::string num = std::to_string(maxSize);
            WriteCharDataT(", [%s x i8]", num.data(), num.length());
        }
    }
    WriteCharDataT(" }\n");
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
        case BuiltIn::bool_t:    WriteCharDataT("i8");   break;
        case BuiltIn::s_char_8:  WriteCharDataT("i8");   break;
        case BuiltIn::u_char_8:  WriteCharDataT("i8");   break;
        case BuiltIn::s_int_16:  WriteCharDataT("i16");  break;
        case BuiltIn::u_int_16:  WriteCharDataT("i16");  break;
        case BuiltIn::s_int_32:  WriteCharDataT("i32");  break;
        case BuiltIn::u_int_32:  WriteCharDataT("i32");  break;
        case BuiltIn::s_int_64:  WriteCharDataT("i64");  break;
        case BuiltIn::u_int_64:  WriteCharDataT("i64");  break;
        case BuiltIn::float_32:  WriteCharDataT("float");   break;
        case BuiltIn::double_64: WriteCharDataT("double");  break;
        case BuiltIn::ptr: WriteCharDataT("ptr");  break;
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
        WriteCharDataT("%s", typeName.data(), typeName.length());
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
            WriteCharDataT(inst.data(), typeName.data(), typeName.length());
        }
        else
        {
            std::string_view inst = symType->dType ==  BuiltIn::struct_t ? "%%struct.%s.%d": "%%union.%s.%d";
            WriteCharDataT(inst.data(), typeName.data(), typeName.length(), typeDesc->second.symbolSaveCounter);
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
                WriteCharDataT("[0 x ");
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
                WriteCharDataT("[%s x ", str.data(), str.length());
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
        WriteCharDataT("ptr");
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

void CodeGen::EmitGlobalVariable(const DeclSpecs *spec, const Declarator *decl)
{
    if(decl->name.length() == 0)
    {
        IssueWarning(&decl->token, "Abstract declarator cannot be emitted");
    }

    WriteByteGV("");

}

void CodeGen::EmitLocalVariable(const DeclSpecs *spec, const Declarator *decl)
{
    IssueWarning(&decl->token, "Local variable is not supported");
}

void CodeGen::WriteByteT(const char* c)
{
    WriteByteImpl(TYPE_BUFFER, c);
}

void CodeGen::WriteByteT(char c)
{
    WriteByteImpl(TYPE_BUFFER, c);
}

void CodeGen::WriteByteGV(const char *c)
{
    WriteByteImpl(GLOB_VAR_BUFFER, c);
}

void CodeGen::WriteByteGV(char c)
{
    WriteByteImpl(GLOB_VAR_BUFFER, c);
}

void CodeGen::WriteByteLV(const char *c)
{
}

void CodeGen::WriteByteLV(char c)
{
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

void CodeGen::WriteCharDataT(const char *data, ...)
{
    va_list args;
    va_start(args, data);
    WriteCharData<TYPE_BUFFER>(data, args);
    va_end(args);
}
