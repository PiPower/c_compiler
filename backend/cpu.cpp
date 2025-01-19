#include "cpu.hpp"
#include "code_gen_utils.hpp"
#include <string.h>
#include <algorithm>
using namespace std;

uint8_t intParamRegs[] = {RDI, RSI, RDX, RCX, R8, R9 };
uint8_t sseParamRegs[] = {XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7 };
uint8_t gpRegsCalleSaved[] = { RBX, R12, R13, R14, R15 };
CpuState *generateCpuState(AstNode *fnDef, SymbolTable *localSymtab, SymbolFunction* symFn)
{
    CpuState* cpu = new CpuState();
    cpu->maxStackSize = 0;
    cpu->currentStackSize = 0;
    cpu->stackArgsOffset = 16;
    cpu->retSignature[0] = 0;
    cpu->retSignature[1] = 0;

    bindReturnValue(cpu, localSymtab, symFn);
    bindArgs(cpu, localSymtab, symFn);
    reserveCalleSavedRegs(cpu);
    bindBlockToStack(cpu, localSymtab);

    return cpu;
}

void bindReturnValue(CpuState *cpu, SymbolTable *localSymtab, SymbolFunction *symFn)
{
    if(*symFn->retType == "void")
    {
        cpu->retSignature[0] = SYSV_NONE;
        cpu->retSignature[1] = SYSV_NONE;
        return;
    }
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
        bindArg(cpu, var, type, *symFn->argNames[i],argClass);
        setIsArg(var);
    }
    
}

void bindArg(CpuState *cpu, SymbolVariable *symVar, SymbolType* symType, const std::string& varname, SysVgrDesc cls)
{
    if(cpu->reg[R8].state == REG_USED && cls.gr[0] == cls.gr[1] && cls.gr[0] == SYSV_INTEGER)
    {
        printf(" NOT SUPPORTED \n");\
        exit(-1);
    }
    
    if(cpu->reg[XMM6].state == REG_USED && cls.gr[0] == cls.gr[1] && cls.gr[0] == SYSV_SSE)
    {
        printf(" NOT SUPPORTED \n");\
        exit(-1);
    }

    for(uint8_t i =0; i < 2; i++)
    {
        if( cls.gr[i] == SYSV_NONE)
        {   
            return;
        }
        if(cls.gr[i] == SYSV_INTEGER)
        {
            char freeRegId = getUnusedArgRegId(cpu);
            if(freeRegId >= 0)
            {
                cpu->reg[freeRegId].state = REG_USED;
                cpu->reg[freeRegId].symbol = varname;
                VariableDesc desc = {Storage::REG, freeRegId};
                cpu->data[varname] = desc;
            }
            else
            {
                bindArgToCpuStack(cpu, symType, varname);
            }
        }  
        else if(cls.gr[i] == SYSV_SSE)
        {
            char freeRegId = getUnusedArgXRegId(cpu);
            if(freeRegId >= 0)
            {
                cpu->reg[freeRegId].state = REG_USED;
                cpu->reg[freeRegId].symbol = varname;
                VariableDesc desc = {Storage::REG, freeRegId};
                cpu->data[varname] = desc;
            }
            else
            {
                bindArgToCpuStack(cpu, symType, varname);
            }
        }
        else if(cls.gr[i] == SYSV_MEMORY)
        {
            bindArgToCpuStack(cpu, symType, varname);
            return;
        }
        else
        {
            printf("Internal error: given group is not supported");
            exit(-1);
        }

    }
}

void reserveCalleSavedRegs(CpuState *cpu)
{
    for(uint8_t i=0;i < 5; i ++)
    {
        cpu->reg[gpRegsCalleSaved[i]].state = REG_USED;
        cpu->reg[gpRegsCalleSaved[i]].symbol = "<calle_saved:" + to_string(i) +'>';
        cpu->data[cpu->reg[gpRegsCalleSaved[i]].symbol] = {Storage::REG, gpRegsCalleSaved[i]};
    }
}

void bindBlockToStack(CpuState* cpu, SymbolTable* localSymtab)
{
    uint32_t blockSize = 0;
    vector<long int*> updates;
    for(size_t i =0; i < localSymtab->symbols.size(); i++)
    {
        Symbol* sym = localSymtab->symbols[i];
        SymbolVariable* symVar = (SymbolVariable*)sym;
        if(localSymtab->symbolNames[i] == "<return_val>" ||
           sym->symClass != SymbolClass::VARIABLE || 
           isSetIsArg(symVar))
        {
            continue;
        }
        SymbolType* type = (SymbolType*)getSymbol(localSymtab, *symVar->varType);
        blockSize += type->typeSize;
        blockSize += blockSize%type->typeSize; // make sure data is aligned
        
        cpu->data[localSymtab->symbolNames[i]] = {Storage::MEMORY,  -(long)blockSize - cpu->currentStackSize};
        updates.push_back(&cpu->data[localSymtab->symbolNames[i]].offset);

    }
    cpu->currentStackSize += blockSize;
    cpu->maxStackSize = std::max(cpu->currentStackSize, cpu->maxStackSize );
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


SysVgrDesc getSysVclass(SymbolTable *localSymtab, SymbolType *type)
{
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

void bindArgToCpuStack(CpuState *cpu, SymbolType* symType, const std::string& varname)
{
    VariableDesc desc = {Storage::MEMORY, cpu->stackArgsOffset};
    cpu->data[varname] = desc;
    uint64_t bytes = symType->typeSize + symType->typeSize % 8;
    cpu->stackArgsOffset += bytes;
}

char getUnusedArgRegId(CpuState* cpu)
{
    for(char freeRegId =0; freeRegId < 6; freeRegId++)
    {
        if(cpu->reg[intParamRegs[freeRegId]].state == REG_FREE)
        {
            return intParamRegs[freeRegId];
        }
    }
    return -1;
}

char getUnusedArgXRegId(CpuState *cpu)
{
    for(char freeRegId =0; freeRegId < 6; freeRegId++)
    {
        if(cpu->reg[sseParamRegs[freeRegId]].state == REG_FREE)
        {
            return sseParamRegs[freeRegId];
        }
    }
    return -1;
}

void fillTypeHwdInfoForBlock(SymbolTable* localSymtab)
{
    for(size_t i =0; i < localSymtab->symbolNames.size(); i++)
    {
        if(localSymtab->symbols[i]->symClass == SymbolClass::TYPE)
        {
            SymbolType* symType = (SymbolType*)localSymtab->symbols[i];
            if(!isSetTranslatedToHwd(symType))
            {
                fillTypeHwdInfo(localSymtab, symType);
            }
        }
    }
}