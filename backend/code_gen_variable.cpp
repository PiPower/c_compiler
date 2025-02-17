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
    string exitLabel = generateLocalPositionLabel();
    string nextBlockLabel;
    AstNode* ifBlock = parseTree;
    do 
    {   
        nextBlockLabel = generateLocalPositionLabel();
        generateConditionCheck(gen, ifBlock->children[0], nextBlockLabel);
        dispatch(gen, ifBlock->children[1]);
        ADD_INST(gen, {INSTRUCTION, "jmp", exitLabel});
        ADD_INST(gen, {LABEL, nextBlockLabel});
        ifBlock = ifBlock->children[2];

    }while(ifBlock && ifBlock->nodeType == NodeType::IF);
    
    if(ifBlock)
    {
        translateBlock(gen, ifBlock);
    }
    ADD_INST(gen, {LABEL, exitLabel});
    return {OP::NONE};
}

OpDesc translateBlock(CodeGenerator *gen, AstNode *block)
{
    SymbolTable* symtabBuff = nullptr;
    if(block->data)
    {
        symtabBuff = gen->localSymtab;
        gen->localSymtab = (SymbolTable*)block->data;
        bindBlockToStack(gen->cpu, gen->localSymtab);
    }
    for (size_t i = 0; i < block->children.size(); i++)
    {
        AstNode* parseTree = block->children[i];
        OpDesc desc = dispatch(gen, parseTree);
        freeRegister(gen, desc.operand);
    }

    if(symtabBuff)
    {
        freeCurrentCpuBlock(gen->cpu);
        gen->localSymtab = symtabBuff;
    }
    return {OP::NONE};
}

OpDesc translateForLoop(CodeGenerator *gen, AstNode *parseTree)
{
    AstNode* init_expr = parseTree->children[0];
    AstNode* cond_expr = parseTree->children[1]; 
    AstNode* update_expr = parseTree->children[2];
    AstNode* compoundStmt = parseTree->children[3];

    SymbolTable* symtabBuff = gen->localSymtab;
    gen->localSymtab = (SymbolTable*)parseTree->data;
    bindBlockToStack(gen->cpu, gen->localSymtab);
    string loopStart = generateLocalPositionLabel();
    string loopEnd = generateLocalPositionLabel();

    if(init_expr)
    {
        FREE_DISPATCH(gen, init_expr);
    }
    ADD_INST(gen, {LABEL, loopStart});
    if(cond_expr)
    {
        generateConditionCheck(gen, cond_expr, loopEnd);
    }
    if(compoundStmt)
    {
        FREE_DISPATCH(gen, compoundStmt);
    }
    if(update_expr)
    {
        FREE_DISPATCH(gen, update_expr);
    }
    ADD_INST(gen, {INSTRUCTION, "jmp", loopStart});
    ADD_INST(gen, {LABEL, loopEnd});

    freeCurrentCpuBlock(gen->cpu);
    gen->localSymtab = symtabBuff;
    return {OP::NONE};
}

OpDesc translateWhileLoop(CodeGenerator *gen, AstNode *parseTree)
{
    string endOfLoop = generateLocalPositionLabel();
    string startOfLoop = generateLocalPositionLabel();

    ADD_INST(gen, {LABEL, startOfLoop});
    generateConditionCheck(gen, parseTree->children[0], endOfLoop);
    dispatch(gen, parseTree->children[1]);
    ADD_INST(gen, {INSTRUCTION, "jmp", startOfLoop});
    ADD_INST(gen, {LABEL, endOfLoop});

    return {OP::NONE};
}

OpDesc translateDoWhileLoop(CodeGenerator *gen, AstNode *parseTree)
{
    string startOfLoop = generateLocalPositionLabel();

    ADD_INST(gen, {LABEL, startOfLoop});
    dispatch(gen, parseTree->children[1]);
    generateConditionCheck(gen, parseTree->children[0], startOfLoop, false);

    return {OP::NONE};
}

OpDesc translateGlobalInit(CodeGenerator *gen, AstNode *parseTree)
{
    //TODO add support for non zero global init
    SymbolVariable* symVar = (SymbolVariable*)GET_SCOPED_SYM(gen, *parseTree->data);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *symVar->varType);
    Instruction inst;
    inst.type = LABEL;
    if(parseTree->children.size() == 0)
    {
        inst.src = generateGlobalProp(symType, *parseTree->data);
        inst.dest = zeroInitVariable(symType);
    }
    else
    {
        inst.src = generateGlobalProp(symType, *parseTree->data, false);
        inst.dest = initVariable(gen, parseTree);
    }
    inst.mnemonic = std::move(*parseTree->data);
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

