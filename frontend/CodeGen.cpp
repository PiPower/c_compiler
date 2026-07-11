#include "CodeGen.hpp"
#include <stdarg.h>
#include "../utils/DataEncoder.hpp"
#include <sys/uio.h>
#include <unistd.h>
#include "SemanticAnalysis.hpp"
#include <string.h>
#include <bit>
#include "../utils/Misc.hpp"
#include "Parser.hpp"
#define VIEW(x) std::string_view(x)
#define IssueWarning(tokenPtr, errorMsg, ...) logger.IssueWarningImpl(tokenPtr, errorMsg __VA_OPT__(,) __VA_ARGS__); exit(-1);

constexpr uint8_t TYPE_BUFFER = 0;
constexpr uint8_t FUNC_BUFFER = 1;
constexpr uint8_t GLOB_VAR_BUFFER = 2;
constexpr uint8_t LOCAL_BUFFER = 3;
constexpr uint8_t STR_BUFFER = 4;
constexpr uint8_t INTRINSIC_BUFFER = 5;
constexpr uint8_t ATTR_BUFFER = 6;

constexpr int FIRST_VALUE = -1;
constexpr int nr_of_pages = 7;
constexpr uint64_t INST_BUFF_SIZE = nr_of_pages * CPU_PAGE_SIZE;
const char* ptrAlignment = "8";
const char* memcpyIntr = "\ndeclare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #%l";


CodeGen::CodeGen(SymbolTable* symTab,  FileManager* manager, NodeExecutor* ne)
:
chosenBuffer(TYPE_BUFFER), currFn(-1, "", false, false), attrCtr(1), typeHeap(nr_of_pages), symTab(symTab),
manager(manager), nodeExec(ne), logger(manager, "Code Gen")
{
    for(size_t i =0 ; i < writableBufferArr.size(); i++)
    {
        currPtrArr[i] = typeHeap.allocateArray<char>(INST_BUFF_SIZE);
        writableBufferArr[i].push_back(currPtrArr[i]);
    }
    BindFuncBuffer();
    WriteByte('\n');
    BindAttrBuffer();
    WriteCharData("\n\n" R"(attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" })");
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
        size_t maxAlignmentMember = 0;

        for(size_t i =0; i < symType->str.argCount; i++)
        {
            if(symType->str.memberList[i].size > maxSize)
            {
                maxSize = symType->str.memberList[i].size;
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
            WriteCharData(inst.data(), typeName.data(), (int)typeName.length(), typeDesc->second.symbolSaveCounter);
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

bool CodeGen::IsBlockTerminated()
{
    return currFn.isBlockTerminated;
}

void CodeGen::EmitGlobalVariable(const DeclSpecs *spec, const Declarator *decl)
{
    BindGlobalVarBuffer();

    if(decl->name.length() == 0)
    {
        IssueWarning(&decl->token, "Abstract declarator cannot be emitted");
    }

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

    EmitDeclarator(&decl->accArr, &spec->typenameView);
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

int64_t CodeGen::AllocateLocalVariable(BuiltIn::Type type, SymbolType* symType, const std::string_view& typeName)
{
    BindFuncBuffer();
    int64_t id = GetIdxForLocalVar();
    if(!isStructOrUnion(type))
    {
        std::string_view typeName = GetBuiltInName(type);
        uint64_t align = GetBuiltInAlignment(type);
        WriteCharData("\n\t%%%l = alloca %v, align %lu", id, typeName, align);
    }
    else
    {
        WriteCharData("\n\t%%%l = alloca ", id);
        EmitTypename(symType, typeName);
        WriteCharData(", align %lu ", symType->alignment);
    }
    return id;
}

void CodeGen::EmitFunctionName(const DeclSpecs *spec, const Declarator *decl, bool declareFunc)
{
    currFn.isBlockTerminated = false;
    currFn.inFunction = true;
    currFn.variableIdx = 0;
    currFn.fnName = decl->name;
    
    std::string_view vis = spec->declType.spec.static_ ? "internal" : "dso_local";

    if(IsArray(&decl->accArr))
    {
        IssueWarning(&decl->token, "Function cannot return array type");
    }
    if(!spec->symType)
    {
        IssueWarning(&decl->token, "EmitFunctionName: unknown return type");
    }

    std::string retName = getRetName(spec, decl);
    bool retByStack = false;
    BindFuncBuffer();

    if(!declareFunc)
    {
        WriteCharData("\ndefine %s %s @%s(", 
                        vis.data(), vis.length(), 
                        retName.data(), retName.length(),
                        decl->name.data(), decl->name.length());
    }
    else
    {
        WriteCharData("\ndeclare %s @%s(", 
                        retName.data(), retName.length(),
                        decl->name.data(), decl->name.length());
    }

}

void CodeGen::EmitReturnByPtr(SymbolType* symType, const std::string_view& typenameView, int8_t flags, int64_t argIdx)
{
    if((flags & fpIsUsedInCall) > 0)
    {
        BindLocalBuffer();
    }
    else
    {
        BindFuncBuffer();
    }

    WriteCharData("ptr dead_on_unwind noalias writable sret(");
    EmitTypename(symType, typenameView, true);
    if(!(flags & fpIsUsedInCall))
    {
        WriteCharData(") align 8 %%0");
        GetIdxForLocalVar();
    }
    else
    {
        WriteCharData(") align 8 %%%l", argIdx);
    }

    if((flags & fpIsLast) == 0)
    {
        WriteCharData(", ");
    }
}

void CodeGen::EmitFunctionParam(BuiltIn::Type type, int8_t flags, Operator op)
{
    if((flags & fpIsUsedInCall) > 0 )
    {
        BindLocalBuffer(); 
    }
    else
    {
        BindFuncBuffer(); 
    }
    std::string_view typeView = GetBuiltInName(type);

    if((flags & fpIsUsedInCall) == 0 && type == BuiltIn::string)
    {
        IssueWarning(nullptr, "String cannot be used in function declaration")
    }

    if(type == BuiltIn::string)
    {
        EmitString(false, op.idx);
    }
    else if(type == BuiltIn::special)
    {
        WriteCharData("...");
    }
    else if(op.idx != EXPR_ID_CONST)
    {
        WriteCharData("%v noundef %%%l", typeView, op.idx);
    }
    else
    {
        std::string constValue = Typed::ToString(op.num);
        WriteCharData("%v noundef %v", typeView, VIEW(constValue));
    }

    if((flags & fpIsLast) == 0 )
    {
        WriteCharData(", ");
    }
}

void CodeGen::EmitFunctionParam(SymbolType* symType, const std::string_view &typeName, int8_t flags, int64_t idx)
{
    if((flags & fpIsUsedInCall) > 0 )
    {
        BindLocalBuffer(); 
    }
    else
    {
        BindFuncBuffer(); 
    }
    
    WriteCharData("ptr noundef byval(");
    EmitTypename(symType, typeName);
    WriteCharData(") align 8 %%%l", idx);

    if((flags & fpIsLast) == 0 )
    {
        WriteCharData(", ");
    }
}

int64_t CodeGen::AllocatePassByTmpStruct(const std::string_view& left, const std::string_view& right, uint64_t alignment)
{
    BindFuncBuffer();
    int64_t idx = GetIdxForLocalVar();
    if(right == "")
    {
        WriteCharData("\n\t%%%l = alloca %v, align %lu", idx, left, alignment);
    }
    else
    {
        WriteCharData("\n\t%%%l = alloca { %v, %v }, align %lu", idx,  left,  right, alignment);
    }

    return idx;
}

void CodeGen::EmitSimpleReturn(BuiltIn::Type dType, Operator ret)
{
    BindLocalBuffer();

    if(dType == BuiltIn::none)
    {
        WriteCharData("\n\tret void");
        return;
    }

    std::string_view typeView = GetBuiltInName(dType);
    if(ret.idx != EXPR_ID_CONST)
    {
        WriteCharData("\n\tret %v %%%l", typeView, ret.idx);
    }
    else
    {
        std::string num = Typed::ToString(ret.num);
        WriteCharData("\n\tret %v %v", typeView, VIEW(num));
    }

}


void CodeGen::CloseParamList()
{
    BindFuncBuffer(); 
    WriteCharData(") #0");
}

void CodeGen::EmitFunctionStart()
{
    BindFuncBuffer(); 
    WriteCharData(" {");
}

void CodeGen::EmitInitializer(const DeclSpecs *spec, const Ast::Node *initializer, bool isComplexType)
{
    if(!(spec->symType->dType >= BuiltIn::s_char_8 && spec->symType->dType <= BuiltIn::double_64))
    {
        IssueWarning(nullptr, "Unsupported initializer type by codegen");
    }
    if(!initializer)
    {
        if(isComplexType)
        {
            WriteCharData(" zeroinitializer");
        }
        else
        {
            WriteCharData(" 0");
            if(isFloat(spec->symType->dType))
            {
                WriteCharData(".0e+00");
            }
        }
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
        if(num.type == Typed::d_dynamic)
        {
            IssueWarning(&initializer->token, "global value may only be initialized by constant expression")
        }

        initStr = Typed::ToString(CastTypedNumber(spec->symType->dType, num));
    }

    if(isFloat(spec->symType->dType))
    {
        initStr += "e+00";
    }
    BindGlobalVarBuffer();
    WriteCharData(" %s", initStr.data(), initStr.length());
}

void CodeGen::EmitGlobalBuiltInInit(const Ast::Node* initExpr, uint32_t alignment)
{
    BindGlobalVarBuffer();
    if(initExpr)
    {
        Typed::Number num = nodeExec->ExecuteNode(initExpr);
        std::string str = Typed::ToString(num);
        std::string alignmentStr = std::to_string(alignment);

        WriteCharData(" %s, align %s",
            str.data(), str.length(),
            alignmentStr.data(), alignmentStr.length());
    }
    else
    {
        std::string alignmentStr = std::to_string(alignment);

        WriteCharData(" zeroinitializer, align %s",
               alignmentStr.data(), alignmentStr.length());
    }
}

void CodeGen::EmitGLobalArrayAlignment(bool isPtr, uint32_t alignment)
{
    BindGlobalVarBuffer();
    if(isPtr)
    {
        WriteCharData(", align 8");
    }
    else
    {
        std::string alignmentStr = std::to_string(alignment);
        WriteCharData(", align %s", alignmentStr.data(), alignmentStr.length());
    }
}

void CodeGen::EmitFunctionClose(BuiltIn::Type retType, int64_t retIdx, int64_t retVal, DeclSpecs* retSpec)
{
    BindLocalBuffer(); 
    EmitLocalLabel(retIdx);
    if(retType == BuiltIn::void_t)
    {
        WriteCharData("\n\tret void");
    }
    else if(!isStructOrUnion(retType))
    {
        std::string_view typeView = GetBuiltInName(retType);
        int64_t retLoad = EmitLocalLoad(retType, GetBuiltInAlignment(retType), retVal);
        WriteCharData("\n\tret %v %%%l", typeView, retLoad);
    }
    else if(retSpec->symType->passByValue)
    {
        std::string retName = getRetName(retSpec, nullptr);
        int64_t tmpIdx =  GetIdxForLocalVar();
        uint64_t alignment = retSpec->symType->alignment;
        WriteCharData("\n\t%%%l = load %v, ptr %%%l align %l", tmpIdx, VIEW(retName), retVal, alignment);
        WriteCharData("\n\tret %v %%%l", VIEW(retName), tmpIdx);
    }
    else
    {
        WriteCharData("\n\tret void");
    }

    BindFuncBuffer(); 
    // copy LOC_VAR_BUFFER buffer to FUNC_BUFFER
    CopyBuffers(FUNC_BUFFER, LOCAL_BUFFER);
    WriteCharData("\n}\n");

    currFn.inFunction = false;
    currFn.variableIdx = -1;
    currFn.fnName = "";
    currFn.isBlockTerminated = false;
    ResetBuffer(LOCAL_BUFFER);
}

void CodeGen::ZeroInitGlobalVar(const DeclSpecs* spec, const Declarator* decl)
{
    BindGlobalVarBuffer();
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

    if(!IsPointer(&decl->accArr) && 
        (spec->symType->dType == BuiltIn::float_32 || spec->symType->dType == BuiltIn::double_64))
    {
        WriteCharData("%se+00, align %s", 
            zero_init.data(), zero_init.length(),
            alignment.data(), alignment.length());
    }
    else
    {
        WriteCharData("%s, align %s", 
            zero_init.data(), zero_init.length(),
            alignment.data(), alignment.length());
    }
}


void CodeGen::InitLocalArray(const std::string_view& arrName, const AccessArray *accArr, const Ast::Node *initExpr, const DeclSpecs *spec)
{
    /*
    PushBufferType();
    BindTmpBuffer();

    WriteCharData("\n@__const.%s.%s = private unnamed_addr constant ",
        currFn.fnName.data(), currFn.fnName.length(),
        arrName.data(), arrName.length());

    EmitDeclarator(accArr, &spec->typenameView);

    CopyBuffers(GLOB_VAR_BUFFER, TMP_BUFFER);
    ResetBuffer(TMP_BUFFER);
    PopBufferType();
    */
}

void CodeGen::EmitLocalBuiltInStorage(BuiltIn::Type type, int32_t alignment, int64_t destIdx, Operator op)
{
    if(op.idx == EXPR_ID_CONST)
    {
        EmitLocalConstAsm(type, alignment, destIdx, op.num);
    }
    else
    {
        EmitLocalStorage(type, alignment, destIdx, op.idx);
    }
}

void CodeGen::EmitLocalStorage(BuiltIn::Type type, int32_t alignment, int64_t destIdx, int64_t srcIdx)
{
    BindLocalBuffer();

    std::string dst = std::to_string(destIdx);
    std::string src = std::to_string(srcIdx);
    std::string align = std::to_string(alignment);

    WriteCharData("\n\tstore ");
    EmitBuiltInTypename(GetBuiltInName(type));
    WriteCharData(" %%%s, ptr", src.data(), src.length());
    WriteCharData(" %%%s, align %s", dst.data(), dst.length(), align.data(), align.length());
}

void CodeGen::EmitLocalStrStorage(int64_t destIdx, int64_t strIdx)
{
    BindLocalBuffer();
    std::string strDest = std::to_string(destIdx);
    std::string strIdxStr = std::to_string(strIdx);
    WriteCharData("\n\tstore ptr @.str.%v, ptr %%%v, align 8", VIEW(strIdxStr), VIEW(strDest));
}

void CodeGen::EmitLocalNullStorage(int64_t destIdx)
{
    BindLocalBuffer();
    std::string strDest = std::to_string(destIdx);
    WriteCharData("\n\tstore ptr null, ptr %%%v, align 8", VIEW(strDest));
}

void CodeGen::EmitLocalNamedStore(BuiltIn::Type type, int32_t alignment, int64_t dstIdx, const std::string_view& name)
{
    BindLocalBuffer();
    std::string strDest = std::to_string(dstIdx);
    std::string_view typeView = GetBuiltInName(type);
    WriteCharData("\n\tstore %v @%v, ptr %%%v, align 8", typeView, name, VIEW(strDest));
}

void CodeGen::EmitLocalConstAsm(BuiltIn::Type type, int32_t alignment, int64_t destIdx, const Typed::Number& num)
{
    BindLocalBuffer();
    std::string strIdx = std::to_string(destIdx);
    std::string strAlign = std::to_string(alignment);
    switch (type)
    {
    case BuiltIn::Type::s_char_8:
    case BuiltIn::Type::u_char_8:
    {
        std::string value = std::to_string(Typed::CastTo<int8_t>(num));
        WriteCharData("\n\tstore i8 %v, ptr %%%v, align %v",
            VIEW(value), VIEW(strIdx), VIEW(strAlign));
    } break;
    case BuiltIn::Type::s_int_16:
    case BuiltIn::Type::u_int_16:
    {
        std::string value = std::to_string(Typed::CastTo<int16_t>(num));
        WriteCharData("\n\tstore i16 %v, ptr %%%v, align %v",
            VIEW(value), VIEW(strIdx), VIEW(strAlign));
    } break;
    case BuiltIn::Type::s_int_32:
    case BuiltIn::Type::u_int_32:
    {
        std::string value = std::to_string(Typed::CastTo<int32_t>(num));
        WriteCharData("\n\tstore i32 %v, ptr %%%v, align %v",
            VIEW(value), VIEW(strIdx), VIEW(strAlign));
    } break;
    case BuiltIn::Type::s_int_64:
    case BuiltIn::Type::u_int_64:
    {
        std::string value = std::to_string(Typed::CastTo<int64_t>(num));
        WriteCharData("\n\tstore i64 %v, ptr %%%v, align %v",
            VIEW(value), VIEW(strIdx), VIEW(strAlign));
    } break;
    case BuiltIn::Type::float_32:
    {
        std::string value = std::to_string(Typed::CastTo<float>(num));
        WriteCharData("\n\tstore float %ve+00, ptr %%%v, align %v",
            VIEW(value), VIEW(strIdx), VIEW(strAlign));
    } break;
    case BuiltIn::Type::double_64:
    {
        std::string value = std::to_string(Typed::CastTo<double>(num));
        WriteCharData("\n\tstore double %ve+00, ptr %%%v, align %v",
            VIEW(value), VIEW(strIdx), VIEW(strAlign));
    } break;
    case BuiltIn::Type::long_double:
    {
        long double value = Typed::CastTo<long double>(num);
        const unsigned char* bytes  = reinterpret_cast<const unsigned char*>(&value);
        WriteCharData("\n\tstore x86_fp80 f0x");
        if constexpr (std::endian::native == std::endian::big)
        {
            for (int i = 0; i <= 9; i++)
            {
                WriteByteInHex(bytes[i + 6]);
            }
        }
        else
        {
            for (int i = 9; i >= 0; i--)
            {
                WriteByteInHex(bytes[i]);
            }
        }
        WriteCharData(", ptr %%%v, align %v", VIEW(strIdx), VIEW(strAlign));
    } break;
    default:
        //IssueWarning(nullptr, "Type assignment is not supported")
        break;
    }
}

void CodeGen::CopyPassTmpStructToStruct(
    int64_t destIdx,
    uint64_t destSize, 
    BuiltIn::Type left, 
    BuiltIn::Type right, 
    uint64_t alignment, 
    int64_t lIdx, 
    int64_t rIdx)
{
    if(right == BuiltIn::none)
    {
        std::string_view passStruct = GetBuiltInName(left);
        BindLocalBuffer();
        WriteCharData("\n\tstore %v %l, ptr %l, align %lu", passStruct, lIdx, destIdx, alignment);
    }
    else
    {
        int64_t passIdx = AllocatePassByTmpStruct(GetBuiltInName(left), GetBuiltInName(right), alignment);
        BindLocalBuffer();
        int64_t firstTypeIdx = GetIdxForLocalVar();
        int64_t secondTypeIdx = GetIdxForLocalVar();

        std::string passStruct = "{ " + std::string(GetBuiltInName(left)) + ", " + std::string(GetBuiltInName(right)) + " }";
        WriteCharData("\n\t%%%l = getelementptr inbounds nuw %v, ptr %%%l, i32 0, i32 0", firstTypeIdx, VIEW(passStruct), passIdx);
        WriteCharData("\n\tstore %v %%%l, ptr %%%l, align %lu", GetBuiltInName(left), lIdx, firstTypeIdx, alignment);
        WriteCharData("\n\t%%%l = getelementptr inbounds nuw %v, ptr %%%l, i32 0, i32 1", secondTypeIdx, VIEW(passStruct), passIdx);
        WriteCharData("\n\tstore %v %%%l, ptr %%%l, align %lu", GetBuiltInName(right), rIdx, secondTypeIdx, alignment);
        EmitLocalIntMemcpy(alignment, alignment, destIdx, passIdx, destSize);
    }
}

int64_t CodeGen::EmitLocalArrGetElemPtr(
    const AccessArray *acc, 
    const std::string_view& typeName, 
    int64_t arrayIdx, 
    const std::vector<uint64_t>& indicies)
{
    BindLocalBuffer();
    int64_t result = GetIdxForLocalVar();
    WriteCharData("\n\t%%%l = getelementptr inbounds ", result);
    if(acc)
    {
        EmitDeclarator(acc, &typeName);
    }
    else
    {
        WriteCharData("%v", typeName);
    }
    WriteCharData(", ptr %%%l, i64 0", arrayIdx);
    for(uint64_t idx : indicies)
    {
        WriteCharData(", i64 %lu", idx);
    }

    return result;
}

void CodeGen::EmitLocalLabel(int64_t label)
{
    BindLocalBuffer();
    currFn.isBlockTerminated = false;
    WriteCharData("\n\nlabel_%l:", label);
}

void CodeGen::EmitLocalJump(int64_t label)
{
    BindLocalBuffer();
    currFn.isBlockTerminated = true;
    WriteCharData("\n\tbr label %%label_%l", label);
}

void CodeGen::EmitLocalCondJump(int64_t cond, int64_t jmpIfTrue, int64_t jmpIfFalse)
{
    BindLocalBuffer();
    currFn.isBlockTerminated = true;
    WriteCharData("\n\tbr i1 %%%l, label %%label_%l, label %%label_%l", cond, jmpIfTrue, jmpIfFalse);
}

int64_t CodeGen::EmitLocalLabel()
{
    BindLocalBuffer();
    int64_t label = GetIdxForLocalVar();
    WriteCharData("\nlabel_%l:", label);
    currFn.isBlockTerminated = false;
    return label;
}

void CodeGen::EmitLocalSwitch(
    BuiltIn::Type type, 
    int64_t cond, 
    int64_t exitLabel, 
    const std::vector<int64_t> &caseLabels, 
    const std::vector<Typed::Number>& labelValues)
{
    BindLocalBuffer();
    if(!isInteger(type))
    {
        IssueWarning(nullptr, "Switch can only be used on integer types")
    }
    if(caseLabels.size() != labelValues.size())
    {
        IssueWarning(nullptr, "Number of label values does not math number of case values")
    }

    std::string_view typeView = GetBuiltInName(type);
    WriteCharData("\n\tswitch %v %%%l, label %%%l [", typeView, cond, exitLabel);
    for(size_t i = 0; i < caseLabels.size(); i++)
    {
        std::string value = Typed::ToString(labelValues[i]);
        WriteCharData("\n\t\t %v %v, label %%%l", typeView, VIEW(value), caseLabels[i]);
    }
    WriteCharData("\n\t]");
    return;
}

int64_t CodeGen::EmitLocalGlLoad(BuiltIn::Type type, int32_t alignment, const std::string_view &varName)
{
    BindLocalBuffer();
    int64_t targetIdx = GetIdxForLocalVar();
    std::string_view srcType = GetBuiltInName(type);

    WriteCharData("\n\t%%%l = load %v, ptr @%v, align %l",
            targetIdx, srcType, varName, alignment);
    return targetIdx;
}

int64_t CodeGen::EmitLocalLoad(BuiltIn::Type type, int32_t alignment, int64_t loadIdx)
{
    std::string_view srcType = GetBuiltInName(type);
    return EmitLocalLoad(srcType, alignment, loadIdx);
}

int64_t CodeGen::EmitLocalLoad(const std::string_view &typeView, int32_t alignment, int64_t loadIdx)
{
    BindLocalBuffer();
    std::string strLoadIdx = std::to_string(loadIdx);
    std::string strAlign = std::to_string(alignment);
    int64_t targetIdx = GetIdxForLocalVar();
    std::string strTargetIdx = std::to_string(targetIdx);

    WriteCharData("\n\t%%%v = load %v, ptr %%%v, align %v",
            VIEW(strTargetIdx), typeView, VIEW(strLoadIdx), VIEW(strAlign));

    return targetIdx;
}

int64_t CodeGen::EmitLocalSignExt(BuiltIn::Type dstType, BuiltIn::Type srcType, int64_t loadIdx)
{
    BindLocalBuffer();
    std::string_view srcTypeView = GetBuiltInName(srcType);
    std::string_view dstTypeView = GetBuiltInName(dstType);
    int64_t targetIdx = GetIdxForLocalVar();
    std::string strTargetIdx = std::to_string(targetIdx);
    std::string strLoadIdx = std::to_string(loadIdx);

    WriteCharData("\n\t%%%v = sext %v %%%v to %v",
            VIEW(strTargetIdx), srcTypeView, VIEW(strLoadIdx), dstTypeView);

    return targetIdx;
}

int64_t CodeGen::EmitLocalZeroExt(BuiltIn::Type dstType, BuiltIn::Type srcType, int64_t loadIdx)
{
    BindLocalBuffer();
    std::string_view srcTypeView = GetBuiltInName(srcType);
    std::string_view dstTypeView = GetBuiltInName(dstType);
    int64_t targetIdx = GetIdxForLocalVar();
    std::string strTargetIdx = std::to_string(targetIdx);
    std::string strLoadIdx = std::to_string(loadIdx);

    WriteCharData("\n\t%%%v = zext %v %%%v to %v",
            VIEW(strTargetIdx), srcTypeView, VIEW(strLoadIdx), dstTypeView);

    return targetIdx;
}

int64_t CodeGen::EmitLocalAddition(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "add", "add", "fadd", true);
}

int64_t CodeGen::EmitLocalSubtraction(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "sub", "sub", "fsub", true);
}

int64_t CodeGen::EmitLocalMultiplication(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "mul", "mul", "fmul", true);
}

int64_t CodeGen::EmitLocalDivision(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "sdiv", "udiv", "fdiv", false);
}

int64_t CodeGen::EmitLocalModulus(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "srem", "urem", "", false);

}

int64_t CodeGen::EmitLocalBitAnd(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "and", "and", "", false);
}

int64_t CodeGen::EmitLocalBitOr(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "or", "or", "", false);
}

int64_t CodeGen::EmitLocalBitXor(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "xor", "xor", "", false);
}

int64_t CodeGen::EmitLocalShiftLeft(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "shl", "shl", "", false);
}

int64_t CodeGen::EmitLocalShiftRight(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "ashr", "lshr", "", false);
}
int64_t CodeGen::EmitLocalIntTruncate(BuiltIn::Type dstType, BuiltIn::Type srcType, Operator src)
{
    if(!isInteger(dstType) || !isInteger(srcType) || src.idx == EXPR_ID_CONST)
    {
        return EXPR_ID_IGNORE;
    }

    std::string_view dstView = GetBuiltInName(dstType);
    std::string_view srcView = GetBuiltInName(srcType);
    int64_t targetIdx = GetIdxForLocalVar();
    std::string strTargetIdx = std::to_string(targetIdx);
    std::string operandIdx = std::to_string(src.idx);

    WriteCharData("\n\t%%%v = trunc %v %%%v to %v",
            VIEW(strTargetIdx), srcView, VIEW(operandIdx), dstView);
    return targetIdx;
}
int64_t CodeGen::EmitLocalCmpGe(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "icmp sgt", "icmp ugt", "fcmp ogt", false);
}
int64_t CodeGen::EmitLocalCmpGeEq(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "icmp sge", "icmp uge", "fcmp oge", false);
}
int64_t CodeGen::EmitLocalCmpLe(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "icmp slt", "icmp ult", "fcmp olt", false);
}
int64_t CodeGen::EmitLocalCmpLeEq(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "icmp sle", "icmp ule", "fcmp ole", false);
}
int64_t CodeGen::EmitLocalCmpEq(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "icmp eq", "icmp eq", "fcmp oeq", false);
}
int64_t CodeGen::EmitLocalCmpNotEq(BuiltIn::Type opType, Operator left, Operator right)
{
    return EmitLocalBinaryOp(opType, left, right, "icmp ne", "icmp ne", "fcmp one", false);
}
int64_t CodeGen::EmitOpenFnCall(BuiltIn::Type ret, std::string_view fnName, const DeclSpecs* spec)
{
    BindLocalBuffer();
    int64_t id = EXPR_ID_IGNORE;
    std::string_view retType;
    if(ret == BuiltIn::special)
    {
        retType = getRetName(spec, nullptr);
    }
    else
    {
        retType = GetBuiltInName(ret);
    }
    

    WriteCharData("\n\t");
    if(ret != BuiltIn::void_t)
    {
        id = GetIdxForLocalVar();
        WriteCharData("%%%l = ", id);
    }
    WriteCharData("call %v @%v(", retType, fnName);

    return id;
}

void CodeGen::EmitCloseFnCall()
{
    BindLocalBuffer();
    WriteByte(')');
}

void CodeGen::EmitZeroInitType(bool isGlobal)
{
    if(isGlobal)
    {
        BindGlobalVarBuffer();
    }
    else
    {
        BindLocalBuffer();
    }
    WriteCharData(" zeroinitializer");
}
void CodeGen::EmitZeroInitInt(bool isGlobal)
{
    if(isGlobal)
    {
        BindGlobalVarBuffer();
    }
    else
    {
        BindLocalBuffer();
    }
    WriteCharData(" 0");
}
void CodeGen::EmitZeroInitFloat(bool isGlobal)
{
    if(isGlobal)
    {
        BindGlobalVarBuffer();
    }
    else
    {
        BindLocalBuffer();
    }
    WriteCharData(" 0.0e+00");
}
void CodeGen::EmitString(bool isGlobal, int64_t strIdx)
{
    if(isGlobal)
    {
        BindGlobalVarBuffer();
    }
    else
    {
        BindLocalBuffer();
    }

    std::string initStr = std::to_string(strIdx);
    WriteCharData("@.str.%v", VIEW(initStr));
}

void CodeGen::EmitConstant(bool isGlobal, BuiltIn::Type dstType, const Typed::Number &num)
{
    Typed::Number numLocal = num;
    if(isFloat(dstType) || isInteger(dstType))
    {
        numLocal = CastTypedNumber(dstType, num);
    }

    std::string value = Typed::ToString(numLocal);
    WriteCharData(" %v", VIEW(value));
}

int64_t CodeGen::EmitLocalBinaryOp(
    BuiltIn::Type opType,
    Operator left,
    Operator right,
    std::string_view opSigned,
    std::string_view opUnsigned,
    std::string_view opFloat,
    bool usePoison)
{
    BindLocalBuffer();
    int64_t targetIdx = GetIdxForLocalVar();
    std::string strTargetIdx = std::to_string(targetIdx);
    std::string strLeft = left.idx == EXPR_ID_CONST ? Typed::ToString(left.num) : std::to_string(left.idx);
    std::string strRight = right.idx == EXPR_ID_CONST ? Typed::ToString(right.num) : std::to_string(right.idx);
    std::string_view strLeftConst = left.idx == EXPR_ID_CONST ? "" : "%";
    std::string_view strRightConst = right.idx == EXPR_ID_CONST ? "" : "%";
    std::string_view opTypeView = GetBuiltInName(opType);
    std::string_view opStr;
    std::string_view poisonVal = "";

    if(isFloat(opType))
    {
        opStr = opFloat;
    }
    else
    {
        opStr = isSigned(opType) ? opSigned : opUnsigned;
    }
    if(usePoison && isSigned(opType))
    {
        poisonVal = " nsw";
    }

    if(opStr == "")
    {
        return EXPR_ID_IGNORE;
    }

    WriteCharData("\n\t%%%v = %v%v %v %v%v, %v%v",
            VIEW(strTargetIdx), opStr, poisonVal, opTypeView, strLeftConst, VIEW(strLeft), strRightConst, VIEW(strRight));
    return targetIdx;
}

int64_t CodeGen::EmitString(const Ast::Node *string)
{
    std::string_view strLiteral = GetViewForToken(string->token, manager);
    auto strIter = emittedStrings.find(strLiteral);
    if(strIter != emittedStrings.end())
    {
        return strIter->second;
    }

    BindStrBuffer();
    static int64_t stringIdx= 1;
    int64_t idx = stringIdx++;
    emittedStrings[strLiteral] = idx;
    std::string strIdx = std::to_string(idx);
    std::string strLen = std::to_string(strLiteral.length() + 1);
    
    WriteCharData("\n@.str.%s = private unnamed_addr constant [%s x i8] c\"",
    strIdx.data(), strIdx.length(), strLen.data(), strLen.length());

    for(size_t i =0; i < strLiteral.length(); i++)
    {
        if(strLiteral[i] == '\\' && strLiteral.length() >= i + 1)
        {
            i++;
            switch (strLiteral[i])
            {
            case '0': WriteCharData("\\00"); break;
            case 'n': WriteCharData("\\0A"); break;
            case 'r': WriteCharData("\\0D"); break;
            case 't': WriteCharData("\\09"); break;
            case '"': WriteCharData("\\22"); break;
            case '\\': WriteCharData("\\5C"); break;
            default: WriteByte(strLiteral[i]); break;
            }
        }
        else
        {
            WriteByte(strLiteral[i]);
        }
    }

    WriteCharData("\\00\", align 1");
    return idx;
}

void CodeGen::EmitLocalIntMemcpy(uint64_t lAlign, uint64_t rAlign, int64_t dest, int64_t src, uint64_t size)
{
    BindLocalBuffer();
    WriteCharData("\n\tcall void @llvm.memcpy.p0.p0.i64(ptr align %lu %%%l, ptr align %lu %%%l, i64 %lu, i1 false)", 
            lAlign, dest, rAlign,  src, size);
    DeclareIntrinsic(Intrinsic::llvm_memcpy);
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
    constexpr std::array<uint8_t, 6> buffOrder = {TYPE_BUFFER, STR_BUFFER, GLOB_VAR_BUFFER, FUNC_BUFFER, INTRINSIC_BUFFER, ATTR_BUFFER};
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

void CodeGen::StartArray()
{
    WriteCharData(" [");
}

void CodeGen::EndArray()
{
    WriteByte(']');
}

void CodeGen::ArgSeparator()
{
    WriteCharData(", ");
}

ByValueStructDesc CodeGen::BuildValueStruct(const StructDesc &desc)
{
    // TODO this code may need improvement for randomized layouts
    int lSize = 0, rSize =0, deltaSize = 0;
    int* consideredSize = &lSize;
    uint32_t alignment = 1;
    size_t i =0;
    while(i < desc.argCount)
    {
        const Member* member = &desc.memberList[i];
        int remainigBytes = *consideredSize % member->alignment ;
        deltaSize = remainigBytes == 0 ? 0 : member->alignment - remainigBytes;
        deltaSize += member->size;
        if(lSize + deltaSize <= 8)
        {
            lSize += deltaSize;
        }
        else if(rSize == 0)
        {
            alignment = 1;
            consideredSize = &rSize;
            rSize += member->size;
        }
        else
        {
            rSize += deltaSize;
        }
        
        alignment = std::max(alignment, member->alignment);
        i++;
    }

    if(lSize + rSize > 16)
    {
        IssueWarning(nullptr, "Internal fata error: lSize + rSize > 16");
    }

    if(rSize == 0)
    {
        switch (lSize)
        {
        case 1: return {"i8", ""};
        case 2: return {"i16", ""};
        case 3: return {"i24", ""};
        case 4: return {"i32", ""};
        case 5: if (alignment < 4) return {"i40", ""}; break;
        case 6: if (alignment < 4) return {"i48", ""}; break;
        case 7: if (alignment < 4) return {"i56", ""}; break;
        }
        return {"i64", ""};
    }
    else
    {
        ByValueStructDesc out = {};
        switch (lSize)
        {
        case 1: out.lType = "i8";   break;
        case 2: out.lType = "i16";  break;
        case 3: 
        case 4: out.lType = "i32";  break;
        default: out.lType = "i64"; break;
        }
        
        switch (rSize)
        {
        case 1: out.rType = "i8";   break;
        case 2: out.rType = "i16";  break;
        case 3: 
        case 4: out.rType = "i32";  break;
        default: out.rType = "i64"; break;
        }

        if(out.rType != "i64")
        {
            out.lType = "i64";
        }

        return out;
    }
    return {};
}

std::string CodeGen::getRetName(const DeclSpecs* spec, const Declarator* decl)
{
    if(decl && IsPointer(&decl->accArr))
    {
        return "ptr";
    }
    else if(spec->symType->passByValue == 1 && 
            (spec->symType->dType == BuiltIn::struct_t || spec->symType->dType == BuiltIn::union_t))
    {
        if(spec->symType->size <= 8)
        {
            return "{ i" + std::to_string(spec->symType->size * 8) + " }";
        }
        else
        {
            ByValueStructDesc desc = BuildValueStruct(spec->symType->str);
            if(desc.lType == "")
            {
                return desc.lType;
            }
            return "{ " + desc.lType + ", " + desc.rType + " }";
        }

    }
    else if(spec->symType->passByValue == 1)
    {
        std::string_view view =  GetBuiltInName(spec->symType->dType);
        std::string ret(view.data(), view.length());
        return ret;
    }
    else
    {
        return "void";
    }
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

void CodeGen::BindStrBuffer()
{
    chosenBuffer = STR_BUFFER;
}

void CodeGen::BindFuncBuffer()
{
    chosenBuffer = FUNC_BUFFER;
}

void CodeGen::BindGlobalVarBuffer()
{
    chosenBuffer = GLOB_VAR_BUFFER;
}

void CodeGen::BindLocalBuffer()
{
    chosenBuffer = LOCAL_BUFFER;
}

void CodeGen::BindIntrinsicBuffer()
{
    chosenBuffer = INTRINSIC_BUFFER;
}

void CodeGen::BindAttrBuffer()
{
    chosenBuffer = ATTR_BUFFER;
}



void CodeGen::DeclareIntrinsic(Intrinsic intr)
{
    
    switch (intr)
    {
    case Intrinsic::llvm_memcpy :
    {
        static bool memcpyEmitted = false;
        if(!memcpyEmitted)
        {
            memcpyEmitted = true;
            BindIntrinsicBuffer();
            int64_t attr = attrCtr++;
            WriteCharData(memcpyIntr, attr);
            BindAttrBuffer();
            WriteCharData("\nattributes #%l = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }", attr);
        }
    }break;
    
    default:
        break;
    }
}

void CodeGen::WriteByte(const char *c)
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

void CodeGen::WriteByteInHex(char c)
{
    static const char* cTable = "0123456789ABCDEF";
    int lIdx = (c >> 4) & 0x0f;
    int rIdx = c & 0x0f;
    WriteByte(cTable[lIdx]);
    WriteByte(cTable[rIdx]);
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
        if(*curr == '\0'){ return;}
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
        case 'v':
        {
            curr++;
            std::string_view view = va_arg(args, std::string_view);
            size_t i = 0;
            while (i < view.size())
            {
                WriteByte(view[i]);
                i++;
            }
        }break;
        case 'l':
        {
            curr++;
            std::string view;
            if(*curr != '0' && *curr == 'u')
            {
                uint64_t data =  va_arg(args, uint64_t);
                view = std::to_string(data);
                curr++;
            }
            else
            {
                int64_t data = va_arg(args, int64_t);
                view = std::to_string(data);
            }

            size_t i = 0;
            while (i < view.size())
            {
                WriteByte(view[i]);
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
