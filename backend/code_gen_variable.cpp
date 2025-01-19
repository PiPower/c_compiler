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
    ADD_INST(gen, {INSTRUCTION, "subq", "", "%rsp"} );
    size_t fillSubInstInd = gen->code.size() - 1;

    AstNode* fnBody = parseTree->children[0];
    for (size_t i = 0; i < fnBody->children.size(); i++)
    {
        AstNode* parseTree = fnBody->children[i];
        dispatch(gen, parseTree);
    }
    gen->code[fillSubInstInd].src = '$' + to_string(gen->cpu->maxStackSize);
    ADD_INST(gen, {INSTRUCTION, "leave"} );
    ADD_INST(gen, {INSTRUCTION, "ret"} );

    gen->localSymtab = gen->localSymtab->parent;
}

void translateDeclaration(CodeGenerator *gen, AstNode *parseTree)
{
    if(gen->localSymtab->scopeLevel == 0)
    {
        translateGlobalInit(gen, parseTree);
    }
    else
    {
        translateLocalInit(gen, parseTree);
    }
}

void translateExpression(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::ASSIGNMENT:
        /* code */
        break;
    
    default:
        break;
    }
}

void prepareVariable(CodeGenerator *gen, AstNode *parseTree)
{
    gen->opDesc.op = OP::VARIABLE;
    gen->opDesc.operand =  *parseTree->data;
}

void prepareConstant(CodeGenerator *gen, AstNode *parseTree)
{
    gen->opDesc.op = OP::CONSTANT;
    gen->opDesc.operand =  *parseTree->data;
}

void writeConstantToSym(CodeGenerator *gen, std::string constant, const std::string& dest)
{
    SymbolVariable* symVar = (SymbolVariable*)GET_SCOPED_SYM(gen, dest);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *symVar->varType);
    uint8_t gr = getTypeGroup(symType->affiliation);
    if(gr == SIGNED_INT_GROUP)
    {

    }
    else if (gr == UNSIGNED_INT_GROUP)
    {
        printf("Internal Error: Unsupported group\n");
        exit(-1);
    }
    else if (gr == FLOAT_GROUP)
    {
        printf("Internal Error: Unsupported group\n");
        exit(-1);
    }
    else
    {
        printf("Internal Error: Unsupported group\n");
        exit(-1);
    }
    
}   

void translateGlobalInit(CodeGenerator *gen, AstNode *parseTree)
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

void translateLocalInit(CodeGenerator *gen, AstNode *parseTree)
{
    CLEAR_OP(gen);
    dispatch(gen, parseTree->children[0]->children[0]);
    writeToLocalVariable(gen, *parseTree->data, gen->opDesc);
}


void writeToLocalVariable(CodeGenerator *gen, const std::string &varname, OpDesc operandDesc)
{
    if(gen->opDesc.op == OP::CONSTANT)
    {
        writeConstantToSym(gen, gen->opDesc.operand, varname);
    }


}
