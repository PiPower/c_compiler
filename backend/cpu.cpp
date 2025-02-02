#include "cpu.hpp"
#include "code_gen_utils.hpp"
#include <string.h>
#include <algorithm>
using namespace std;

uint8_t intParamRegs[] = {RDI, RSI, RDX, RCX, R8, R9 };
uint8_t sseParamRegs[] = {XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7 };
uint8_t gpRegsCalleSaved[] = { RBX, R12, R13, R14, R15 };

const char* cpu_registers_str[][5] = {
        {"rax", "eax", "ax","al", "ah"}, 
        {"rbx", "ebx", "bx", "bl","bh"}, 
        {"rcx", "ecx", "cx", "cl","ch"},
        {"rdx", "edx", "dx", "dl","dh"}, 
        {"rsi", "esi", "si", "sil"},  
        {"rdi", "edi", "di", "dil"}, 
        {"rsp", "esp", "sp", "spl"},   
        {"rbp", "ebp", "bp", "bpl"}, 
        {"r8", "r8d", "r8w", "r8b"}, 
        {"r9", "r9d", "r9w", "r9b"}, 
        {"r10", "r10d", "r10w", "r10b"}, 
        {"r11", "r11d", "r11w", "r11b"}, 
        {"r12", "r12d", "r12w", "r12b"}, 
        {"r13", "r13d", "r13w", "r13b"}, 
        {"r14", "r14d", "r14w", "r14b"}, 
        {"r15", "r15d", "r15w", "r15b"}, 
        {"zmm0", "ymm0", "xmm0"},
        {"zmm1", "ymm1", "xmm1"},
        {"zmm2", "ymm2", "xmm2"},
        {"zmm3", "ymm3", "xmm3"},
        {"zmm4", "ymm4", "xmm4"},
        {"zmm5", "ymm5", "xmm5"},
        {"zmm6", "ymm6", "xmm6"},
        {"zmm7", "ymm7", "xmm7"},
        {"zmm8", "ymm8", "xmm8"},
        {"zmm9", "ymm9", "xmm9"},
        {"zmm10", "ymm10", "xmm10"},
        {"zmm11", "ymm11", "xmm11"},
        {"zmm12", "ymm12", "xmm12"},
        {"zmm13", "ymm13", "xmm13"},
        {"zmm14", "ymm14", "xmm14"},
        {"zmm15", "ymm15", "xmm15"},
        {"rip", "eip", "ip"},
        {"rflags", "eflags"},
        {0,0, "cs"},
        {0,0, "ds"},
        {0,0, "es"},
        {0,0, "ss"},
        {0,0, "fs"},
        {0,0, "gs"},

    };

CpuState *generateCpuState(AstNode *fnDef, SymbolTable *localSymtab, SymbolFunction* symFn)
{
    CpuState* cpu = new CpuState();
    cpu->maxStackSize = 0;
    cpu->currentStackSize = 0;
    cpu->stackArgsOffset = 16;
    cpu->retSignature[0] = 0;
    cpu->retSignature[1] = 0;
    cpu->runtimeStackSize = 0;
    cpu->reg[RSP].state = REG_FORBIDDEN;
    cpu->reg[RBP].state = REG_FORBIDDEN;
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
        cpu->regData[cpu->reg[RDI].symbol ] = {Storage::REG, RDI};
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
            signed char freeRegId = getUnusedArgRegId(cpu);
            if(freeRegId >= 0)
            {
                cpu->reg[freeRegId].state = REG_USED;
                cpu->reg[freeRegId].symbol = varname;
                VariableCpuDesc desc = {Storage::REG, freeRegId};
                cpu->regData[varname] = desc;
            }
            else
            {
                bindArgToCpuStack(cpu, symType, varname);
            }
        }  
        else if(cls.gr[i] == SYSV_SSE)
        {
            signed char freeRegId = getUnusedArgMMRegId(cpu);
            if(freeRegId >= 0)
            {
                cpu->reg[freeRegId].state = REG_USED;
                cpu->reg[freeRegId].symbol = varname;
                VariableCpuDesc desc = {Storage::REG, freeRegId};
                cpu->regData[varname] = desc;
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
        cpu->regData[cpu->reg[gpRegsCalleSaved[i]].symbol] = {Storage::REG, gpRegsCalleSaved[i]};
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
        if(blockSize%type->dataAlignment) // make sure data is aligned
        {
            blockSize+= type->dataAlignment - blockSize%type->dataAlignment;
        }
        cpu->stackData[localSymtab->symbolNames[i]] = {Storage::STACK,  -(long)blockSize - cpu->currentStackSize};
        updates.push_back(&cpu->stackData[localSymtab->symbolNames[i]].offset);

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
    for(size_t i =0; i < symType->names.size(); i++)
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
    VariableCpuDesc desc = {Storage::STACK, cpu->stackArgsOffset};
    cpu->stackData[varname] = desc;
    uint64_t bytes = symType->typeSize;
    if(symType->typeSize % 8)
    {
        bytes += 8 - symType->typeSize;
    }

    cpu->stackArgsOffset += bytes;
}

char getUnusedArgRegId(CpuState* cpu)
{
    for(signed char freeRegId =0; freeRegId < 6; freeRegId++)
    {
        if(cpu->reg[intParamRegs[freeRegId]].state == REG_FREE)
        {
            return intParamRegs[freeRegId];
        }
    }
    return -1;
}

char getUnusedArgMMRegId(CpuState *cpu)
{
    for(signed char freeRegId =0; freeRegId < 6; freeRegId++)
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

void increaseStackSize(CpuState *cpu, int64_t size)
{
}

std::string getCpuRegStr(uint8_t regIdx, uint8_t regSize)
{
    return cpu_registers_str[regIdx][regSize];
}

VariableCpuDesc fetchVariable(const CpuState* cpu, const std::string &varName)
{
    auto regVarIter = cpu->regData.find(varName);
    if(regVarIter != cpu->regData.end())
    {
        VariableCpuDesc out = {
            .storageType = regVarIter->second.storageType,
            .offset = regVarIter->second.offset
        };
        return out;
    }

    auto stackVarIter = cpu->stackData.find(varName);
    if(stackVarIter != cpu->stackData.end())
    {
        VariableCpuDesc out = {
            .storageType = stackVarIter->second.storageType,
            .offset = stackVarIter->second.offset
        };
        return out;
    }

    return { .storageType = Storage::MEMORY};
}

std::string generateLocalConstantLabel()
{
    static uint64_t id = 0;
    return ".LC" + to_string(id++);
}

bool isRegisterUsed(const CpuState* cpu, uint8_t regIdx)
{
    return cpu->reg[regIdx].state == REG_USED;
}

bool registerStores(const CpuState* cpu, uint8_t regIdx, const std::string &varName)
{
    if(cpu->reg[regIdx].state == REG_USED)
    {
        return cpu->reg[regIdx].symbol == varName;
    }
    return false;
}



