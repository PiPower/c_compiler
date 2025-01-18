#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
#include <string>
using namespace std;


void translateFunction(CodeGenerator *gen, AstNode *parseTree)
{
    gen->localSymtab = (SymbolTable*)parseTree->children[0]->data;
    fillTypeHwdInfoForBlock(gen->symtab);
    SymbolFunction* symFn = (SymbolFunction*)GET_SCOPED_SYM(gen, *parseTree->data);
    gen->cpu = generateCpuState(parseTree, gen->localSymtab, symFn);

    Instruction inst = generateFunctionLabel(parseTree);
    ADD_INST_MV(gen, inst);
    // function preambule
    ADD_INST(gen, {INSTRUCTION, "pushq", "%rbp", ""} );
    ADD_INST(gen, {INSTRUCTION, "movq", "%rsp", "%rbp"} );
    ADD_INST(gen, {INSTRUCTION, "subq", "", "rsp"} );
    size_t fillSubInstInd = gen->code.size() - 1;

    AstNode* fnBody = parseTree->children[0];
    for (size_t i = 0; i < fnBody->children.size(); i++)
    {
        AstNode* parseTree = fnBody->children[i];
        dispatcher(gen, parseTree);
    }
    
    ADD_INST(gen, {INSTRUCTION, "leave"} );
    ADD_INST(gen, {INSTRUCTION, "ret"} );

    gen->localSymtab = gen->localSymtab->parent;
}

void translateDeclaration(CodeGenerator *gen, AstNode *parseTree)
{
    if(gen->localSymtab->scopeLevel == 0)
    {
        emitGlobalVariable(gen, parseTree);
    }
}

void emitGlobalVariable(CodeGenerator *gen, AstNode *parseTree)
{
    SymbolVariable* symVar = (SymbolVariable*)GET_SCOPED_SYM(gen, *parseTree->data);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *symVar->varType);
    Instruction inst;
    inst.type = LABEL;
    inst.mnemonic = std::move(*parseTree->data);
    if(parseTree->children.size() == 0)
    {
        zeroInitVariable(&inst, symType, inst.mnemonic);
    }
    ADD_INST_MV(gen, inst);
    FREE_NODE_REC(gen, parseTree);
}
