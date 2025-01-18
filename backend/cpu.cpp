#include "cpu.hpp"
#include "code_gen_utils.hpp"
#include <string.h>
#include <algorithm>
using namespace std;

uint8_t intParamRegs[] = {RDI, RSI, RDX, RCX, R8, R9 };
uint8_t sseParamRegs[] = {XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7 };

CpuState *generateCpuState(AstNode *fnDef, SymbolTable *localSymtab, SymbolFunction* symFn)
{
    CpuState* cpu = new CpuState();
    cpu->frameSize = 0;
    cpu->maxStackSize = 0;
    cpu->currentStackSize = 0;
    cpu->stackArgsSize = 0;
    cpu->retSignature[0] = 0;
    cpu->retSignature[1] = 0;

    bindReturnValue(cpu, localSymtab, symFn);
    bindArgs(cpu, localSymtab, symFn);

    return cpu;
}

void bindReturnValue(CpuState *cpu, SymbolTable *localSymtab, SymbolFunction *symFn)
{
    SymbolType* type = (SymbolType*)getSymbol(localSymtab, *symFn->retType);
    SysVgrDesc retGroup = getSysVclass(localSymtab, type);
    uint8_t retINT = RET_RAX;
    uint8_t retSSE = RET_XMM0;

    if(retGroup.gr[0] == RET_MEM)
    {
        cpu->reg[RDI].state = REG_USED;
        cpu->reg[RDI].symbol = "<retPtrToStruct>";
        cpu->retSignature[0] = retGroup.gr[0];
        cpu->retSignature[1] = retGroup.gr[1];
        cpu->data[cpu->reg[RDI].symbol ] = {Storage::REG, RDI};
        return;
    }

    for(uint8_t i =0; i < 2; i++)
    {
        if(retGroup.gr[i] == SYSV_NONE)
        {
            return;
        }
        else if(retGroup.gr[i] == SYSV_INTEGER)
        {
            cpu->retSignature[i] = retINT;
            retINT <<=1;
        }
        else if(retGroup.gr[i] == SYSV_SSE)
        {
            cpu->retSignature[i] = retSSE;
            retSSE <<=1;
        }
    }
}

void bindArgs(CpuState *cpu, SymbolTable *localSymtab, SymbolFunction *symFn)
{

    for (size_t i = 0; i < symFn->argNames.size(); i++)
    {
        SymbolType* type = (SymbolType*)getSymbol(localSymtab, *symFn->argTypes[i]);
        SymbolVariable* var = (SymbolVariable*)getSymbol(localSymtab, *symFn->argNames[i]);
        SysVgrDesc argClass = getSysVclass(localSymtab, type);
        bindArg(cpu, var, *symFn->argNames[i],argClass);
    }
    
}

void bindArg(CpuState *cpu, SymbolVariable *symVar, const std::string& varname, SysVgrDesc cls)
{
    if(cpu->reg[R8].state == REG_USED && cls.gr[0] == cls.gr[1] && cls.gr[0] == SYSV_INTEGER)
    {

    }
    
    if(cpu->reg[XMM6].state == REG_USED && cls.gr[0] == cls.gr[1] && cls.gr[0] == SYSV_SSE)
    {
        
    }

    for(uint8_t i =0; i < 2; i++)
    {
        if( cls.gr[i] == SYSV_NONE)
        {   
            return;
        }
        if(cls.gr[i] == SYSV_INTEGER)
        {
            uint8_t freeRegId;
            for(freeRegId =0; freeRegId < 7; freeRegId++)
            {
                if(freeRegId == 7)
                {
                    break;
                }
                if(cpu->reg[intParamRegs[freeRegId]].state == REG_FREE)
                {
                    break;
                }
            }
            if(freeRegId < 7)
            {
                cpu->reg[intParamRegs[freeRegId]].state = REG_USED;
                VariableDesc desc = {Storage::REG, intParamRegs[freeRegId]};
                cpu->data[varname] = desc;
            }
        }  
        else if(cls.gr[i] == SYSV_SSE)
        {

        }
        else if(cls.gr[i] == SYSV_MEMORY)
        {

        }
        else
        {
            printf("Internal error: given group is not supported");
            exit(-1);
        }

    }
}

void fillTypeHwdInfo(SymbolTable *localSymtab, SymbolType* symType)
{
    if(isBuiltInType(symType))
    {
        // in this case we have built in type
        setTranslatedToHwd(symType);
        return;
    }

    symType->typeSize = 0;
    SymbolType* subType;
    for(int i =0; i < symType->names.size(); i++)
    {
        subType = (SymbolType*)getSymbol(localSymtab, symType->types[i]);
        if(!isSetTranslatedToHwd(subType))
        {
            fillTypeHwdInfo(localSymtab, subType);
        }
        symType->dataAlignment = max(subType->dataAlignment, symType->dataAlignment);
        uint64_t mod = symType->typeSize % subType->dataAlignment;
        if(mod != 0)
        {
            symType->typeSize += subType->dataAlignment - mod;
        }
        symType->paramOffset.push_back(symType->typeSize);
        symType->typeSize += subType->typeSize;
    }
    uint64_t mod = symType->typeSize % symType->dataAlignment;
    if(mod != 0)
    {
        symType->typeSize  += symType->dataAlignment - mod;
    }

    setTranslatedToHwd(symType);
}

//TODO currently does not support arrays
SysVgrDesc tryPackToRegisters(SymbolTable *localSymtab, SymbolType *symType)
{
    int i = 1;
    SysVgrDesc out = {SYSV_NONE, SYSV_NONE};
    uint8_t lowRegClass = SYSV_NO_CLASS;
    uint8_t hiRegClass = SYSV_NO_CLASS;
    while (true)
    {
        FieldDesc atomFieldDesc = getNthFieldDesc(localSymtab, symType, i);
        if(atomFieldDesc.type == nullptr)
        {
            break;
        }

        SymbolType* fieldSym = atomFieldDesc.type; // MUST BE one of base types
        uint32_t offset = atomFieldDesc.offset;
        if((offset%fieldSym->dataAlignment != 0)|| // data field unaligned 
            (offset < 8 && offset + fieldSym->typeSize > 8)) // data field crosses 8byte boundary
        {
            return {SYSV_NONE, SYSV_NONE};
        }

        if( offset < 8)
        {
            SysVgrDesc desc = getSysVclass(localSymtab, fieldSym);
            lowRegClass = resolveSysVclass(lowRegClass, desc.gr[0]);
        }
        else if( offset < 16)
        {
            SysVgrDesc desc = getSysVclass(localSymtab, fieldSym);
            hiRegClass = resolveSysVclass(hiRegClass, desc.gr[0]);
        }
        i++;
    }

    return {lowRegClass, hiRegClass};
}

uint8_t is8ByteAligned(SymbolTable *localSymtab, SymbolType *symType)
{
    return 0;
}

SysVgrDesc getSysVclass(SymbolTable *localSymtab, SymbolType *type)
{
    if(!isSetTranslatedToHwd(type))
    {
        fillTypeHwdInfo(localSymtab, type);
    }

    if(isBuiltInType(type))
    {
        if( (type->affiliation & INT_S_MASK) > 0 ||
            (type->affiliation & INT_U_MASK) > 0 ||
            type->affiliation == POINTER_GR
        )
        {
            return {SYSV_INTEGER, SYSV_NONE};
        }
        if((type->affiliation & FLOAT_MASK) > 0 &&
            type->affiliation != DOUBLE128)
        {
            return {SYSV_SSE, SYSV_NONE};
        }

        if(type->affiliation == DOUBLE128)
        {
            printf("Internal error: DOUBLE128 is not currently supported\n");
            exit(-1);
        }
    }

    if(type->typeSize <= 16)
    {
       SysVgrDesc packed = tryPackToRegisters(localSymtab, type);
       if(packed.gr[0] != SYSV_NONE)
       {
        return packed;
       }
    }
    return {SYSV_MEMORY, type->typeSize};
}

uint8_t resolveSysVclass(uint8_t cl1, uint8_t cl2)
{
    if(cl1 == cl2)
    {
        return cl2;
    }
    if(cl1 == SYSV_NO_CLASS )
    {
        return cl2;
    }
    if(cl2 == SYSV_NO_CLASS )
    {
        return cl1;
    }
    if(cl1 == SYSV_MEMORY || cl2 == SYSV_MEMORY )
    {
        return SYSV_MEMORY;
    }
    if(cl1 == SYSV_INTEGER || cl2 == SYSV_INTEGER )
    {
        return SYSV_INTEGER;
    }
    if((cl1 & (0b111 << 3)) > 0 ||  (cl2 & (0b111 << 3)) > 0 )
    {
        return SYSV_MEMORY;
    }

    return SYSV_SSE;
}

void putVariableIntoCpuData(CpuState *cpu, VariableDesc desc, const std::string& varname)
{
    auto iter = cpu->data.find(varname);
    if(iter == cpu->data.cend())
    {

        cpu->data[varname] = std::move(desc);
        return;
    }

    
}

void bindStructToStack(CpuState *cpu, SymbolTable *localSymtab, SymbolVariable *symVar)
{
}
