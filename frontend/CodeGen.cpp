#include "CodeGen.hpp"

CodeGen::CodeGen(SymbolTable* symTab)
:
typeHeap(7), symTab(symTab)
{

}

void CodeGen::EmitUnionStruct(SymbolType *symType)
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

    for(size_t i =0; i < symType->argCount; i++)
    {
        EmitMember(&symType->memberList[i]);
        Member& currMember = symType->memberList[i];
    }
    

    int x = 2;
}

void CodeGen::EmitMember(Member *member)
{
    if(member->memberType == BuiltIn::struct_t)
    {
        //emittedTypes.find(member->typeName);
        WriteCharData("%%struct.%s");
    }
}

void CodeGen::WriteCharData(const char *data)
{

}
