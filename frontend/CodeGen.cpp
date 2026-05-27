#include "CodeGen.hpp"
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <sys/uio.h>
#include <unistd.h>
#include "../utils/Misc.hpp"
#include "SemanticAnalysis.hpp"
#include <string.h>
#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

constexpr uint8_t TYPE_BUFFER = 0;
constexpr uint8_t FUNC_BUFFER = 1;
constexpr uint8_t GLOB_VAR_BUFFER = 2;
constexpr uint8_t LOC_VAR_BUFFER = 3;

constexpr int FIRST_VALUE = -1;
constexpr int nr_of_pages = 7;
constexpr uint64_t INST_BUFF_SIZE = nr_of_pages * CPU_PAGE_SIZE;

CodeGen::CodeGen(SymbolTable* symTab,  FileManager* manager, NodeExecutor* ne)
:
chosenBuffer(TYPE_BUFFER), currFn(false, -1, ""), typeHeap(nr_of_pages), symTab(symTab),
manager(manager), nodeExec(ne), logger(manager, "Code Gen")
{
    for(size_t i =0 ; i < writableBufferArr.size(); i++)
    {
        currPtrArr[i] = typeHeap.allocateArray<char>(INST_BUFF_SIZE);
        writableBufferArr[i].push_back(currPtrArr[i]);
    }
    BindFuncBuffer();
    WriteByte('\n');
}

void CodeGen::EmitUnionStruct(SymbolType *symType, const std::string_view& name, bool flushQueue)
{
    if(!symType)
    {
        IssueWarning(nullptr, "EmitUnionStruct: null symbol type");
    }
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
    BindTypeBuffer();

    EmitTypename(symType, name, false);
    WriteCharData(" = type { ");
    if(symType->dType == BuiltIn::struct_t)
    {
        for(size_t i =0; i < symType->str.argCount; i++)
        {
            EmitMember(&symType->str.memberList[i]);
            if( i < symType->str.argCount - 1)
            {
                WriteByte(',');
                WriteByte(' ');
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

        // enforces proper alignment 
        EmitMember(&symType->str.memberList[maxAlignmentMember]);
        //size_t aling = maxSize % maxAlignment;
        //maxSize += aling ? maxAlignment - aling : 0;
        maxSize -= symType->str.memberList[maxAlignmentMember].size;
        if(maxSize > 0)
        {
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
        return EmitBuiltInTypename(symType);
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
        symType->str.codeGenIdx = ANON_EMITTED;
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
        
        symType->str.codeGenIdx = typeDesc->second.symbolSaveCounter;
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

void CodeGen::EmitBuiltInTypename(SymbolType *symType)
{
    std::string_view builInType= GetBuiltInName(symType);
    WriteCharData("%s", builInType.data(), builInType.length());
}

std::string_view CodeGen::GetBuiltInName(SymbolType *symType)
{
    switch (symType->dType)
    {
    case BuiltIn::bool_t:      return "i8";   break;
    case BuiltIn::s_char_8:    return "i8";   break;
    case BuiltIn::u_char_8:    return "i8";   break;
    case BuiltIn::s_int_16:    return "i16";  break;
    case BuiltIn::u_int_16:    return "i16";  break;
    case BuiltIn::s_int_32:    return "i32";  break;
    case BuiltIn::u_int_32:    return "i32";  break;
    case BuiltIn::s_int_64:    return "i64";  break;
    case BuiltIn::u_int_64:    return "i64";  break;
    case BuiltIn::float_32:    return "float";   break;
    case BuiltIn::double_64:   return "double";  break;
    case BuiltIn::long_double: return "x86_fp80";  break;
    case BuiltIn::ptr:         return "ptr";     break;
    case BuiltIn::void_t:      return "void";     break;
    default:
        printf("code gen: type unsupported");
        exit(-1);
        break;
    }
    return nullptr;
}

bool CodeGen::EmitDeclarator(const AccessArray* acc, const std::string_view* typeName, const AccessArray* typedefAcc)
{
    static std::vector<std::string_view> arrSizes;

    if(!acc)
    {
        IssueWarning(nullptr, "EmitDeclarator: null access chain");
    }
    if(!typeName)
    {
        IssueWarning(nullptr, "EmitDeclarator: null type name");
    }

    if(acc->count == 0 && !(typedefAcc && typedefAcc->count != 0))
    {
        SymbolType* st = symTab->QueryTypeSymbol(*typeName);
        if(!st)
        {
            IssueWarning(nullptr, "EmitDeclarator: unknown type for typename");
        }
        EmitTypename(st, *typeName);
        return false;
    }
    
    return EmitDeclaratorAcc(acc, typeName, typedefAcc);
}

bool CodeGen::EmitDeclaratorAcc(const AccessArray* acc, const std::string_view* typeName, const AccessArray* typedefAcc)
{
    uint64_t brackets = 0;
    bool endedWithPtr = EmitAccessArrayOpened(acc, &brackets);
    if(!endedWithPtr)
    {
        endedWithPtr = EmitAccessArrayOpened(typedefAcc, &brackets);
    }

    if(endedWithPtr)
    {
        WriteCharData("ptr");
    }
    else
    {
        SymbolType* st = symTab->QueryTypeSymbol(*typeName);
        if(!st)
        {
            IssueWarning(nullptr, "EmitDeclarator: unknown type for typename");
        }
        EmitTypename(st, *typeName);
    }

    for(uint32_t i =0; i < brackets; i++)
    {
        WriteByte(']');
    }

    return endedWithPtr;
}

void CodeGen::EmitMember(Member *member)
{
    EmitDeclarator(&member->accArr, &member->typeName, &member->typedefAccArr);
}

bool CodeGen::EmitAccessArrayOpened(const AccessArray *accArr, uint64_t* bracket)
{
    if(!accArr)
    {
        return false;
    }

    for(size_t i = 0; i < accArr->count; i++)
    {
        const AccessType* accType = &accArr->ptr[i];
        if(accType->type == ACC_POINTER)
        {
            return true;
        }
        else if(accType->type == ACC_ARRAY_VLA)
        {
            WriteCharData("[0 x ");
            (*bracket)++;
        }
        else if(accType->type == ACC_ARRAY)
        {
            std::string str = std::to_string(accType->array.size);
            WriteCharData("[%s x ", str.data(), str.length());
            (*bracket)++;
        }
        else
        {
            IssueWarning(nullptr, "Internal: Function calls are not allowed in declaration \n")
        }
    }

    return false;
}

void CodeGen::EmitGlobalVariable(const DeclSpecs *spec, const Declarator *decl)
{
    if(decl->name.length() == 0)
    {
        IssueWarning(&decl->token, "Abstract declarator cannot be emitted");
    }
    BindGlobalVarBuffer();

    if(spec->declType.spec.static_ && currFn.inFunction)
    {
        WriteCharData("\n@%s.%s = internal global ", 
            currFn.fnName.data(), currFn.fnName.length(),
            decl->name.data(), decl->name.length());
    }
    else
    {
        std::string_view vis = spec->declType.spec.static_ ? "internal" : "dso_local";

        WriteCharData("\n@%s = %s global ", 
                    decl->name.data(), decl->name.length(),
                    vis.data(), vis.length() );
    }

    bool isPtr = EmitDeclarator(&decl->accArr, &spec->typenameView, spec->accArr);

    if(decl->initExpr)
    {
        InitGlobalVar(spec, decl, isPtr);
    }
    else 
    {
        ZeroInitGlobalVar(spec, decl);
    }
}

void CodeGen::EmitLocalVariable(const DeclSpecs *spec, const Declarator *decl, const Ast::Node* initExpr)
{
    const SymbolVariable* symVar = symTab->QueryVarSymbol(decl->name);
    if(!symVar)
    {
        IssueWarning(&decl->token, "EmitLocalVariable: unknown variable symbol");
    }
    const SymbolType* varType = symVar->spec.symType;
    if(!varType)
    {
        IssueWarning(&decl->token, "EmitLocalVariable: variable has no type");
    }
    std::string idx = std::to_string(symVar->varIdx);

    //ProcessedDecl procDecl = ProcessDecl(&decl->accessTypes, &spec->typenameView, spec->acc);
    //procDecl.variableIdx = symVar->varIdx;
    //Initializer init = ProcessInitExpr(initExpr);
}

void CodeGen::EmitFunctionName(const DeclSpecs *spec, const Declarator *decl)
{
    currFn.inFunction = true;
    currFn.variableIdx = 1;
    currFn.fnName = decl->name;
    
    std::string_view vis = spec->declType.spec.static_ ? "internal" : "dso_local";
    BindGlobalVarBuffer();

    if(IsArray(&decl->accArr))
    {
        IssueWarning(&decl->token, "Function cannot return array type");
    }
    if(!spec->symType)
    {
        IssueWarning(&decl->token, "EmitFunctionName: unknown return type");
    }
    if( (spec->symType->dType == BuiltIn::struct_t ||  
            spec->symType->dType == BuiltIn::union_t) &&
        !IsPointer(&decl->accArr))
    {
        IssueWarning(&decl->token, "Internal: Complex types are not supported");
    }
    if(spec->symType->dType == BuiltIn::struct_t ||
       spec->symType->dType == BuiltIn::union_t)
    {
        IssueWarning(nullptr, "Non built in types are not supported in return statement");
    }
    std::string_view retName = GetBuiltInName(spec->symType);

    BindFuncBuffer();
    WriteCharData("\ndefine %s %s @%s() #0 {", 
                    vis.data(), vis.length(), 
                    retName.data(), retName.length(),
                    decl->name.data(), decl->name.length());
}

void CodeGen::EmitFunctionClose()
{
    BindFuncBuffer(); 
    // copy LOC_VAR_BUFFER buffer to FUNC_BUFFER
    std::vector<char*>& destVec = writableBufferArr[chosenBuffer];
    std::vector<char*>& srcVec = writableBufferArr[LOC_VAR_BUFFER];
    for(size_t i = 0; srcVec.size() - 1; i++)
    {   
        size_t usedBytes = currPtrArr[chosenBuffer] - destVec.back();
        size_t freeBytes = INST_BUFF_SIZE - usedBytes;
        memcpy(destVec.back(), srcVec[i], freeBytes);
        
        destVec.push_back(typeHeap.allocateArray<char>(INST_BUFF_SIZE));
        memcpy(destVec.back(), srcVec[i] + freeBytes, INST_BUFF_SIZE - freeBytes);
        currPtrArr[chosenBuffer] = destVec.back() + INST_BUFF_SIZE - freeBytes;
    }

    size_t usedBytes = currPtrArr[chosenBuffer] - destVec.back();
    size_t freeBytes = INST_BUFF_SIZE - usedBytes;
    size_t copySize = std::min(freeBytes, (size_t)(currPtrArr[LOC_VAR_BUFFER] - srcVec.back()));
    memcpy(destVec.back(), srcVec.back(), copySize);
    if(freeBytes < (size_t)(currPtrArr[LOC_VAR_BUFFER] - srcVec.back()) )
    {
        size_t remainingBytes =  (size_t)(currPtrArr[LOC_VAR_BUFFER] - srcVec.back()) - freeBytes;
        destVec.push_back(typeHeap.allocateArray<char>(INST_BUFF_SIZE));
        memcpy(destVec.back(), srcVec.back() + copySize, remainingBytes);
        currPtrArr[chosenBuffer] = destVec.back() + remainingBytes;
    }
    else
    {   
        currPtrArr[chosenBuffer] += copySize;
    }

    WriteCharData("\n}\n");
    // reset state of LOC_VAR_BUFFER;
    for(size_t i = 1; i < writableBufferArr[LOC_VAR_BUFFER].size(); i++)
    {
        localBufferHandle.push_back(writableBufferArr[LOC_VAR_BUFFER][i]);
    }
    writableBufferArr[LOC_VAR_BUFFER].resize(1);
    currPtrArr[LOC_VAR_BUFFER] = writableBufferArr[LOC_VAR_BUFFER][0];

    currFn.inFunction = false;
    currFn.variableIdx = -1;
    currFn.fnName = "";
}

void CodeGen::InitGlobalVar(const DeclSpecs *spec, const Declarator *decl, bool isPtr)
{
    if(!isPtr && (spec->symType->dType == BuiltIn::struct_t || spec->symType->dType == BuiltIn::union_t ))
    {
        IssueWarning(nullptr, "Struct/union initialization is not supported")
    }

    if(!spec->accArr && decl->accArr.count == 0)
    {
        Typed::Number num = nodeExec->ExecuteNode(decl->initExpr);
        std::string str = Typed::ToString(num);
        std::string alignment = std::to_string(spec->symType->alignment);

        WriteCharData(" %s, align %s",
            str.data(), str.length(),
            alignment.data(), alignment.length());
    }
    int x = 2;
}

void CodeGen::ZeroInitGlobalVar(const DeclSpecs* spec, const Declarator* decl)
{
    SymbolType* symType = symTab->QueryTypeSymbol(spec->typenameView);
    if(!symType)
    {
        IssueWarning(&decl->token, "EmitGlobalVariable: unknown type for variable");
    }
    std::string_view zero_init;
    std::string alignment;

    if(IsArray(&decl->accArr) || symType->dType == BuiltIn::struct_t ||  symType->dType == BuiltIn::union_t)
    {
        zero_init = " zeroinitializer";
    }
    alignment = std::to_string(symType->alignment);

    WriteCharData("%s, align %s", 
        zero_init.data(), zero_init.length(),
        alignment.data(), alignment.length());
}

std::string_view CodeGen::GetViewForToken(const Token &token)
{
    FILE_STATE state;
    if(manager->GetFileState(&token.location.id, &state) != 0)
    {
        IssueWarning(&token, "CodeGen: could not resolve file state for token view");
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
    constexpr std::array<uint8_t, 3> buffOrder = {TYPE_BUFFER, GLOB_VAR_BUFFER, FUNC_BUFFER,};
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

int64_t CodeGen::GetIdxForLocalVar()
{
    return currFn.variableIdx++;
}

void CodeGen::BindTypeBuffer()
{
    chosenBuffer = TYPE_BUFFER;
}

void CodeGen::BindFuncBuffer()
{
    chosenBuffer = FUNC_BUFFER;
}

void CodeGen::BindGlobalVarBuffer()
{
    chosenBuffer = GLOB_VAR_BUFFER;
}

void CodeGen::BindLocalVarBuffer()
{
    chosenBuffer = LOC_VAR_BUFFER;
}

void CodeGen::WriteByte( const char *c)
{
    WriteByte(*c);
}

void CodeGen::WriteByte(char c)
{
    std::vector<char*>* buffs = &writableBufferArr[chosenBuffer];
    char* currPtr = currPtrArr[chosenBuffer];
    if((uint64_t)(currPtr - buffs->back()) == typeHeap.GetAllocSize())
    {
        if(chosenBuffer == LOC_VAR_BUFFER && localBufferHandle.size() > 0)
        {
            currPtrArr[chosenBuffer] = localBufferHandle.back();
            localBufferHandle.pop_back();
        }
        else
        {
            currPtrArr[chosenBuffer] = typeHeap.allocateArray<char>(nr_of_pages * CPU_PAGE_SIZE);
        }
        currPtr = currPtrArr[chosenBuffer];
        buffs->push_back(currPtrArr[chosenBuffer]);
    }

    *currPtr = c;
    currPtrArr[chosenBuffer]++;
}


void CodeGen::WriteCharData(const char* data, ...)
{
    va_list args;
    va_start(args, data);

    const char* curr = data;

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

