#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
void translateFunction(CodeGenerator *gen, AstNode *parseTree)
{
    
}

void translateDeclaration(CodeGenerator *gen, AstNode *parseTree)
{
    if(gen->scopedSymtab->scopeLevel == 0)
    {
        emitGlobalVariable(gen, parseTree);
    }
}

void emitGlobalVariable(CodeGenerator *gen, AstNode *parseTree)
{
    SymbolVariable* symVar = (SymbolVariable*)GET_SCOPED_SYM(gen, *parseTree->data);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *symVar->varType);
    if(symType->typeSize == 0)
    {
        fillTypeHwdInfo(symType);
    }
    Instruction inst;
    inst.mnemonic = std::move(*parseTree->data);
    inst.mnemonic += ':';

    ADD_INST_MV(gen, inst);
}
