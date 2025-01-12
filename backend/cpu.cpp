#include "cpu.hpp"
#include "code_gen_utils.hpp"
#include <string.h>
#include <algorithm>
using namespace std;

CpuState *generateCpuState(AstNode *fnDef, SymbolTable *localSymtab, SymbolFunction* symFn)
{
    CpuState* cpu = new CpuState();
    cpu->frameSize = 0;
    cpu->retSignature[0] = 0;
    cpu->retSignature[1] = 0;
    bindReturnValue(cpu, localSymtab, symFn);

    for (size_t i = 0; i < symFn->argNames.size(); i++)
    {
        SymbolType* type = (SymbolType*)getSymbol(localSymtab, *symFn->argTypes[i]);
        if(isSetTranslatedToHwd(type))
        {
            fillTypeHwdInfo(localSymtab, type);
        }
    }
    


    return cpu;
}

void bindReturnValue(CpuState *cpu, SymbolTable *localSymtab, SymbolFunction *symFn)
{
    SymbolType* type = (SymbolType*)getSymbol(localSymtab, *symFn->retType);
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
            cpu->retSignature[0] = RET_RAX;
            return;
        }
        if((type->affiliation & FLOAT_GROUP) > 0 &&
            type->affiliation != DOUBLE128)
        {
            cpu->retSignature[0] = RET_XMM0;
            return;
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

    uint64_t currOffset = 0;
    SymbolType* subType;
    for(int i =0; i < symType->names.size(); i++)
    {
        subType = (SymbolType*)getSymbol(localSymtab, symType->types[i]);
        symType->dataAlignment = max(subType->dataAlignment, symType->dataAlignment);
        uint64_t mod = currOffset % subType->dataAlignment;
        if(mod != 0)
        {
            currOffset += subType->dataAlignment - mod;
        }
        symType->paramOffset.push_back(currOffset);
        currOffset += 1;
    }
    symType->typeSize = currOffset - 1 + subType->typeSize;
    uint64_t mod = symType->typeSize % symType->dataAlignment;
    if(mod != 0)
    {
        symType->typeSize  += symType->dataAlignment - mod;
    }
}


uint8_t getSysVGroup(SymbolType *type)
{
    if(isBuiltInType(type))
    {
        if( (type->affiliation & INT_S_MASK) > 0 ||
            (type->affiliation & INT_U_MASK) > 0 ||
            type->affiliation == POINTER_GR
        )
        {
            return SYSV_INTEGER;
        }
        if((type->affiliation & FLOAT_GROUP) > 0 &&
            type->affiliation != DOUBLE128)
        {
            return SYSV_SSE;
        }
    }
    return 0;
}
