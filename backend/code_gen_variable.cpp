#include "code_gen_internal.hpp"

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
    int x = 2;
}
