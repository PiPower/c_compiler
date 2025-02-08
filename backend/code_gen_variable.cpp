#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
#include <string>
using namespace std;


OpDesc translateFunction(CodeGenerator *gen, AstNode *parseTree)
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
        OpDesc desc = dispatch(gen, parseTree);
        freeRegister(gen, desc.operand);
    }
    gen->code[fillSubInstInd].src = '$' + to_string(gen->cpu->maxStackSize);
    ADD_INST(gen, {INSTRUCTION, "leave"} );
    ADD_INST(gen, {INSTRUCTION, "ret"} );

    gen->localSymtab = gen->localSymtab->parent;
    return {OP::NONE};
}

OpDesc translateDeclaration(CodeGenerator *gen, AstNode *parseTree)
{
    if(gen->localSymtab->scopeLevel == 0)
    {
        return translateGlobalInit(gen, parseTree->children[0]);
    }
    else
    {
        return translateLocalInit(gen, parseTree->children[0]);
    }
    return {OP::NONE};
}

OpDesc prepareConstant(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = {
        .operandType =  OP::CONSTANT,
        .operand =  *parseTree->data,
        .operandAffi = 0,
        .scope = 0
    };
    return opDesc;
}

OpDesc prepareVariable(CodeGenerator *gen, AstNode *parseTree)
{
    return parseEncodedAccess(gen,  *parseTree->data);
}


OpDesc processChild(CodeGenerator *gen, AstNode *parseTree, std::size_t child_index, bool loadConst, bool loadVarToReg)
{
    if( parseTree->children[child_index]->nodeType == NodeType::IDENTIFIER )
    {
        OpDesc varDesc = prepareVariable(gen, parseTree->children[child_index]);
        VariableCpuDesc var = fetchVariable(gen->cpu, varDesc.operand);

        if(loadVarToReg && var.storageType != Storage::REG)
        {
            SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
            OpDesc destDesc = {
                .operandType = OP::VARIABLE,
                .operand = varDesc.operand + generateRegisterName(),
                .scope = gen->localSymtab->scopeLevel
            };
            uint8_t gr = getTypeGroup(symType->affiliation);
            if(gr == FLOAT_GROUP)
            {
                destDesc.operandAffi = symType->affiliation;
                allocateMMRegister(gen, destDesc.operand);
            }
            else
            {
                destDesc.operandAffi =  gr == SIGNED_INT_GROUP ? INT64_S: INT64_U;
                allocateRRegister(gen, destDesc.operand);
            }
            varDesc = writeVariableToReg(gen, varDesc, destDesc);
        }

        return varDesc;
    }
    else if(loadConst && parseTree->children[child_index]->nodeType == NodeType::CONSTANT)
    {
        SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
        OpDesc constant = prepareConstant(gen, parseTree->children[child_index]);
        uint8_t gr = getTypeGr(symType->affiliation);
        OpDesc destDesc;
        if(gr == FLOAT_GROUP)
        {
            destDesc = generateTmpVar(symType->affiliation, gen->localSymtab->scopeLevel);
            allocateMMRegister(gen, destDesc.operand);
        }
        else
        {
            destDesc = generateTmpVar(gr == SIGNED_INT_GROUP ? INT64_S: INT64_U, gen->localSymtab->scopeLevel);
            allocateRRegister(gen, destDesc.operand);
        }
        return writeConstant(gen, constant.operand, destDesc);
    }
    
    return dispatch(gen, parseTree->children[child_index]);
}

OpDesc translateIfStmt(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc cond = processChild(gen, parseTree, 0);
    string exitLabel = generateLocalPositionLabel();
    if( parseTree->children[0]->nodeType >= NodeType::LESS && 
        parseTree->children[0]->nodeType <= NodeType::NOT_EQUAL)
    {
        SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
        Instruction jmp = {
            .type = INSTRUCTION,
            .src =exitLabel
        };
        gen->code[gen->code.size() - 1] = jmp;
    }
    return OpDesc();
}

OpDesc translateGlobalInit(CodeGenerator *gen, AstNode *parseTree)
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
    return {OP::NONE};
}


OpDesc translateLocalInit(CodeGenerator *gen, AstNode *parseTree)
{
    AstNode* init = parseTree->children[0]; 
    OpDesc operandDesc = processChild(gen, init, 0, false);

    if(operandDesc.operandType == OP::CONSTANT)
    {
        OpDesc destDesc = parseEncodedAccess(gen,  *parseTree->data);
        writeConstant(gen, operandDesc.operand, destDesc);
    }
    else
    {
        OpDesc destDesc = parseEncodedAccess(gen,  *parseTree->data);
        writeRegToMem(gen, operandDesc, destDesc);
    }
    freeRegister(gen, operandDesc.operand);
    return {OP::NONE};
}

