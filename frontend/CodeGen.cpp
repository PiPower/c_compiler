#include "CodeGen.hpp"
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <sys/uio.h>
#include <unistd.h>
#include "SemanticAnalysis.hpp"
#include <string.h>
#include "Parser.hpp"
#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

constexpr uint8_t TYPE_BUFFER = 0;
constexpr uint8_t FUNC_BUFFER = 1;
constexpr uint8_t GLOB_VAR_BUFFER = 2;
constexpr uint8_t LOC_VAR_BUFFER = 3;
constexpr uint8_t TMP_BUFFER = 4;

constexpr int FIRST_VALUE = -1;
constexpr int nr_of_pages = 7;
constexpr uint64_t INST_BUFF_SIZE = nr_of_pages * CPU_PAGE_SIZE;
const char* ptrAlignment = "8";

CodeGen::CodeGen(SymbolTable* symTab,  FileManager* manager, NodeExecutor* ne)
:
chosenBuffer(TYPE_BUFFER), currFn(false, -1, ""), utilHeap(5), typeHeap(nr_of_pages), symTab(symTab),
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
        return EmitBuiltInTypename(GetBuiltInName(symType->dType));
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

void CodeGen::EmitBuiltInTypename(const std::string_view& builInType)
{
    WriteCharData("%s", builInType.data(), builInType.length());
}

bool CodeGen::EmitDeclarator(const AccessArray* acc, const std::string_view* typeName)
{
    static std::vector<std::string_view> arrSizes;

    if(!acc)
    {
        IssueWarning(nullptr, "EmitDeclarator: null access chain");
    }


    if(acc->count == 0 )
    {
        SymbolType* st = symTab->QueryTypeSymbol(*typeName);
        if(!st)
        {
            IssueWarning(nullptr, "EmitDeclarator: unknown type for typename");
        }
        EmitTypename(st, *typeName);
        return false;
    }

    return EmitDeclaratorAcc(acc, typeName);
}

bool CodeGen::EmitDeclaratorAcc(const AccessArray* acc, const std::string_view* typeName)
{
    uint64_t brackets = 0;
    bool endedWithPtr = EmitAccessArrayOpened(acc, &brackets);

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
    EmitDeclarator(&member->accArr, &member->typeName);
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
            std::string str = accType->array.size != CG_ZERO_SIZED_ARRAY ?
                                std::to_string(accType->array.size):
                                "0";
        
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
    BindTmpBuffer();

    if(spec->declType.spec.static_ && currFn.inFunction)
    {
        WriteCharData("\n@%s.%s = internal global ", 
            currFn.fnName.data(), currFn.fnName.length(),
            decl->name.data(), decl->name.length());
    }
    else
    {
        std::string_view vis = spec->declType.spec.static_ ? "internal" : "dso_local";
        vis = spec->declType.spec.extern_ ? "external" : vis;

        WriteCharData("\n@%s = %s global ", 
                    decl->name.data(), decl->name.length(),
                    vis.data(), vis.length() );
    }

    bool isPtr = EmitDeclarator(&decl->accArr, &spec->typenameView);
    InitGlobalVar(spec, decl, isPtr);

    CopyBuffers(GLOB_VAR_BUFFER, TMP_BUFFER);
    ResetBuffer(TMP_BUFFER);
}

void CodeGen::EmitLocalVariable(const SymbolVariable* symVar)
{
    const SymbolType* varType = symVar->spec.symType;
    if(!varType)
    {
        IssueWarning(&symVar->decl.token, "EmitLocalVariable: variable has no type");
    }
    std::string idx = std::to_string(symVar->varIdx);

    BindFuncBuffer();

    WriteCharData("\n\t%%%s = alloca ", idx.data(), idx.length());
    bool isPtr = EmitDeclarator(&symVar->decl.accArr, &symVar->spec.typenameView);
    std::string alignment = isPtr ? ptrAlignment : std::to_string(varType->alignment);
    WriteCharData(", align %s", alignment.data(), alignment.length());
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
    std::string_view retName = GetBuiltInName(spec->symType->dType);

    BindFuncBuffer();
    WriteCharData("\ndefine %s %s @%s() #0 {", 
                    vis.data(), vis.length(), 
                    retName.data(), retName.length(),
                    decl->name.data(), decl->name.length());
}

void CodeGen::EmitInitializer(const DeclSpecs *spec, const Ast::Node *initializer)
{
    if(!(spec->symType->dType >= BuiltIn::s_char_8 && spec->symType->dType <= BuiltIn::double_64))
    {
        IssueWarning(nullptr, "Unsupported initializer type by codegen");
    }
    if(!initializer)
    {
        WriteCharData(" 0");
        return;
    }
    std::string initStr;
    if(initializer->type == Ast::string_literal)
    {
        int64_t strIdx = EmitString(initializer);
        initStr = "@.str." + std::to_string(strIdx);
    }
    else
    {
        Typed::Number num = nodeExec->ExecuteNode(initializer);
        initStr = Typed::ToString(num);
    }

    WriteCharData(" %s", initStr.data(), initStr.length());
}

void CodeGen::EmitFunctionClose()
{
    BindFuncBuffer(); 
    // copy LOC_VAR_BUFFER buffer to FUNC_BUFFER
    CopyBuffers(FUNC_BUFFER, LOC_VAR_BUFFER);
    WriteCharData("\n}\n");

    currFn.inFunction = false;
    currFn.variableIdx = -1;
    currFn.fnName = "";

    ResetBuffer(LOC_VAR_BUFFER);
}

void CodeGen::InitGlobalVar(const DeclSpecs *spec, const Declarator *decl, bool isPtr)
{
    if(!decl->initExpr)
    {
        ZeroInitGlobalVar(spec, decl);
        return;
    }

    if(!isPtr && (spec->symType->dType == BuiltIn::struct_t || spec->symType->dType == BuiltIn::union_t ))
    {
        IssueWarning(nullptr, "Struct/union initialization is not supported")
    }

    if(decl->accArr.count == 0)
    {
        Typed::Number num = nodeExec->ExecuteNode(decl->initExpr);
        std::string str = Typed::ToString(num);
        std::string alignment = std::to_string(spec->symType->alignment);

        WriteCharData(" %s, align %s",
            str.data(), str.length(),
            alignment.data(), alignment.length());

        return;
    }

    if(!decl->initExpr)
    {
        std::string alignment = std::to_string(spec->symType->alignment);

        WriteCharData(" zeroinitializer, align %s",
               alignment.data(), alignment.length());

        return;
    }
    // array stuff
    InitGlobalArray(&decl->accArr, decl->initExpr, spec);
    if(IsPointer(&decl->accArr))
    {
        WriteCharData(", align 8");
    }
    else
    {
        std::string alignment = std::to_string(spec->symType->alignment);
        WriteCharData(", align %s", alignment.data(), alignment.length());
    }
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
    if(!IsPointer(&decl->accArr))
    {
        alignment = std::to_string(symType->alignment);
    }
    else
    {
        alignment = ptrAlignment;
    }

    WriteCharData("%s, align %s", 
        zero_init.data(), zero_init.length(),
        alignment.data(), alignment.length());
}

void CodeGen::InitGlobalArray(const AccessArray* accArr, const Ast::Node* initExpr, const DeclSpecs *spec)
{
    if(spec->symType->dType == BuiltIn::struct_t || spec->symType->dType == BuiltIn::union_t )
    {
        IssueWarning(nullptr, "Array of non built-in types are not supported")
    }

    // init arrays
    const AccessArray nextAcc = {accArr->ptr + 1, accArr->count - 1};
    bool isNestedArray = nextAcc.count > 0 && IsArray(&nextAcc);

     // here we emit type
    if(accArr->count == 0 || (accArr->ptr[0].type == ACC_POINTER))
    {
        EmitInitializer(spec, initExpr->lChild->lChild);
        return;
    }

    std::vector<ArrayInitPair> pairs;
    if(initExpr)
    {   
        pairs = PartitionArrayInitializer(initExpr, &nextAcc, &logger, nodeExec, &utilHeap);
    }

    
    uint64_t currentPair = 0;
    uint64_t arraySize = accArr->ptr->array.size;
    
    WriteCharData(" [");
    for(uint64_t i = 0; i < arraySize; i++)
    {

        EmitDeclaratorAcc(&nextAcc, &spec->typenameView);
        ArrayInitPair* initPair = nullptr;
        if(currentPair < pairs.size() && i == pairs[currentPair].idx)
        {
            InitGlobalArray(&nextAcc, pairs[currentPair].initializerList, spec);
            currentPair++;
        }
        else
        {
            if(isNestedArray)
            {
                WriteCharData(" zeroinitializer");
            }
            else
            {
                EmitTypename(spec->symType, spec->typenameView, true);
                EmitInitializer(spec, nullptr);
            }
        }

        if(i < arraySize - 1)
        {
            WriteCharData(", ");
        }
    }
    WriteByte(']');

}

void CodeGen::InitLocalArray(const std::string_view& arrName, const AccessArray *accArr, const Ast::Node *initExpr, const DeclSpecs *spec)
{
    PushBufferType();
    BindTmpBuffer();

    WriteCharData("\n@__const.%s.%s = private unnamed_addr constant ",
        currFn.fnName.data(), currFn.fnName.length(),
        arrName.data(), arrName.length());

    EmitDeclarator(accArr, &spec->typenameView);
    InitGlobalArray(accArr, initExpr, spec);

    CopyBuffers(GLOB_VAR_BUFFER, TMP_BUFFER);
    ResetBuffer(TMP_BUFFER);
    PopBufferType();
}

void CodeGen::EmitStorage(BuiltIn::Type type, int32_t alignment, int64_t destIdx, int64_t srcIdx)
{
    PushBufferType();
    BindLocalVarBuffer();

    std::string dst = std::to_string(destIdx);
    std::string src = std::to_string(srcIdx);
    std::string align = std::to_string(alignment);

    WriteCharData("\n\tstore ");
    EmitBuiltInTypename(GetBuiltInName(type));
    WriteCharData(" %%%s, ",dst.data(), dst.length());
    EmitBuiltInTypename(GetBuiltInName(type));
    WriteCharData(" %%%s, align %s", src.data(), src.length(), align.data(), align.length());

    PopBufferType();
}

int64_t CodeGen::EmitString(const Ast::Node *string)
{
    static int64_t stringIdx= 1;
    int64_t idx = stringIdx++;

    std::string strIdx = std::to_string(idx);
    std::string_view strLiteral = GetViewForToken(string->token);
    std::string strLen = std::to_string(strLiteral.length());
    
    PushBufferType();
    BindGlobalVarBuffer(); 

    WriteCharData("\n@.str.%s = private unnamed_addr constant [%s x i8] c\"",
    strIdx.data(), strIdx.length(), strLen.data(), strLen.length());
    WriteCharData("\", align 1");

    PopBufferType();
    return idx;
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

void CodeGen::PushBufferType()
{
    buffStack.push(chosenBuffer);
}

void CodeGen::PopBufferType()
{
    chosenBuffer = buffStack.top();
    buffStack.pop();
}

void CodeGen::BindTypeBuffer()
{
    chosenBuffer = TYPE_BUFFER;
}

void CodeGen::BindTmpBuffer()
{
    chosenBuffer = TMP_BUFFER;
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
        if(allocatedBufferHandles[chosenBuffer].size() > 0)
        {
            currPtrArr[chosenBuffer] = allocatedBufferHandles[chosenBuffer].back();
            allocatedBufferHandles[chosenBuffer].pop_back();
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

void CodeGen::CopyBuffers(uint8_t dest, uint8_t src)
{
        // copy LOC_VAR_BUFFER buffer to FUNC_BUFFER
    std::vector<char*>& destVec = writableBufferArr[dest];
    std::vector<char*>& srcVec = writableBufferArr[src];
    for(size_t i = 0; srcVec.size() - 1; i++)
    {   
        size_t usedBytes = currPtrArr[dest] - destVec.back();
        size_t freeBytes = INST_BUFF_SIZE - usedBytes;
        memcpy(destVec.back(), srcVec[i], freeBytes);
        
        destVec.push_back(typeHeap.allocateArray<char>(INST_BUFF_SIZE));
        memcpy(destVec.back(), srcVec[i] + freeBytes, INST_BUFF_SIZE - freeBytes);
        currPtrArr[dest] = destVec.back() + INST_BUFF_SIZE - freeBytes;
    }

    size_t usedBytes = currPtrArr[dest] - destVec.back();
    size_t freeBytes = INST_BUFF_SIZE - usedBytes;
    size_t copySize = std::min(freeBytes, (size_t)(currPtrArr[src] - srcVec.back()));
    memcpy(destVec.back() + usedBytes, srcVec.back(), copySize);
    if(freeBytes < (size_t)(currPtrArr[src] - srcVec.back()) )
    {
        size_t remainingBytes =  (size_t)(currPtrArr[src] - srcVec.back()) - freeBytes;
        destVec.push_back(typeHeap.allocateArray<char>(INST_BUFF_SIZE));
        memcpy(destVec.back(), srcVec.back() + copySize, remainingBytes);
        currPtrArr[dest] = destVec.back() + remainingBytes;
    }
    else
    {   
        currPtrArr[dest] += copySize;
    }

}

void CodeGen::ResetBuffer(uint8_t buff)
{
    for(size_t i = 1; i < writableBufferArr[buff].size(); i++)
    {
        allocatedBufferHandles[buff].push_back(writableBufferArr[buff][i]);
    }
    writableBufferArr[buff].resize(1);
    currPtrArr[buff] = writableBufferArr[buff][0];
}
