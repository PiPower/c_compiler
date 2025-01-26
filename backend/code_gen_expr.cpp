#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
using namespace std;

OpDesc translateExpr(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::MINUS:
        return translateNegation(gen, parseTree);
    case NodeType::ASSIGNMENT:
        return translateAssignment(gen, parseTree);
    case NodeType::CAST:
        return translateCast(gen, parseTree);
    }
}

OpDesc translateNegation(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = processChild(gen, parseTree, 0);
    if(opDesc.operandType == OP::CONSTANT)
    {
       opDesc.operand.insert(0, 1, '-');
       return opDesc;
    }

    VariableCpuDesc cpuDesc = fetchVariable(gen->cpu, opDesc.operand);
    return opDesc;
}

OpDesc translateAssignment(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = processChild(gen, parseTree, 1);

}

OpDesc translateCast(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = processChild(gen, parseTree, 0);
    //uint8_t gr = getTypeGroup()
}

void moveConstantInt(CodeGenerator *gen, const std::string &constant, const OpDesc &destDesc)
{
    //first process value to be sure it is in correct range
    long int value = encodeIntAsBinary(constant);
    unsigned long int* v2 = (unsigned long int*)&value;
    Instruction inst;
    inst.type = INSTRUCTION;
    int regIdx = -1;
    if(destDesc.operandAffi == INT8_S || destDesc.operandAffi == INT8_U)
    {
        inst.mnemonic = "movb";
        inst.src = encodeIntAsString(value, 1);
    }
    else if(destDesc.operandAffi  == INT16_S || destDesc.operandAffi == INT16_U)
    {
        inst.mnemonic = "movw";
        inst.src = encodeIntAsString(value, 2);
    }
    else if(destDesc.operandAffi  == INT32_S  || destDesc.operandAffi == INT32_U)
    {
        inst.mnemonic = "movl";
        inst.src = encodeIntAsString(value, 4);
    }
    else if(destDesc.operandAffi  == INT64_S ||  destDesc.operandAffi == INT64_U)
    {
        VariableCpuDesc varCpu = fetchVariable(gen->cpu, destDesc.operand);
        if( (uint64_t)(value >> 32 ) > 0)
        {
            if(varCpu.storageType != Storage::REG)
            {
                regIdx = (int)allocateRRegister(gen, "init_tmp");
                Instruction instLocal = {INSTRUCTION, "movabsq", 
                                    encodeIntAsString(value, 8), 
                                    cpu_registers_str[regIdx][0] };
                instLocal.dest = '%' + instLocal.dest;
                inst.src = instLocal.dest;
                inst.mnemonic = "movq";
                ADD_INST_MV(gen, instLocal);
            }
            else
            {
                inst.mnemonic = "movabsq";
            }
        }
        else
        {   
            inst.mnemonic = "movq";
            inst.src = encodeIntAsString(value, 8);
        }
    }

    inst.dest = generateOperand(gen->cpu, destDesc);
    ADD_INST_MV(gen, inst);
    freeRRegister(gen, regIdx);
}

void moveConstantFloat(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc)
{
    int reg = -1;
    OpDesc tempDesc = { .operandType = OP::NONE};
    VariableCpuDesc varCpu = fetchVariable(gen->cpu, destDesc.operand);
    if(varCpu.storageType != Storage::REG)
    {
        tempDesc = {
            .operandType = OP::TEMP_VAR,
            .operand =  "init_float_tmp",
            .operandAffi = destDesc.operandAffi,
            .scope = destDesc.scope
        };
        reg = (int)allocateMMRegister(gen, "init_float_tmp");
    }

    string label;
    uint64_t val = 0;
    Instruction movToReg = {
        .type = INSTRUCTION
    }; 

    if(destDesc.operandAffi == FLOAT32)
    {
        val = encodeFloatAsBinary(constant, 4);
        movToReg.mnemonic = "movss";
    }
    else if(destDesc.operandAffi == DOUBLE64 )
    {
        val = encodeFloatAsBinary(constant, 8);
        movToReg.mnemonic = "movsd";
    }
    else
    {
        printf("Internal error: Unsupported float type \n");
        exit(-1);
    }

    auto floatConst = gen->floatConsts.find(val);
    if(floatConst == gen->floatConsts.cend())
    {
        label = generateLocalConstantLabel();
        gen->floatConsts[val] = label;
    }
    else
    {
        label = gen->floatConsts[val];
    }

    movToReg.src = label + "(%rip)";
    if(tempDesc.operandType == OP::NONE)
    {
        movToReg.dest = generateOperand(gen->cpu, destDesc);
    }
    else
    {
        movToReg.dest = generateOperand(gen->cpu, tempDesc);
    }

    ADD_INST_MV(gen, movToReg);
    if(tempDesc.operandType != OP::NONE)
    {
        const Instruction& prev = gen->code.back();
        Instruction moveToTarget = {
            .type = INSTRUCTION,
            .mnemonic = prev.mnemonic,
            .src = prev.dest,
            .dest = generateOperand(gen->cpu, destDesc)
        };
        ADD_INST_MV(gen, moveToTarget);
    }
    freeMMRegister(gen, reg);
}

void loadSignedInt(CodeGenerator *gen, const OpDesc &varDesc)
{
    string src = generateOperand(gen->cpu, varDesc);
    Instruction loadInst;
    uint8_t reg = allocateRRegister(gen, varDesc.operand);
    switch (varDesc.operandAffi)
    {
    case INT8_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movsbq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, varDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT16_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movswq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, varDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT32_S:
        if(!registerStores(gen->cpu, RAX, varDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "pushq", "%rax"});
        }
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movl",
            .src = std::move(src),
            .dest = "%eax"
        };
        ADD_INST_MV(gen, loadInst);
        ADD_INST(gen, {INSTRUCTION, "cltq"});
        if(!registerStores(gen->cpu, RAX, varDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "movq", "%rax", generateOperand(gen->cpu, varDesc)});
            ADD_INST(gen, {INSTRUCTION, "popq", "%rax"});
        }
        break;
    case INT64_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, varDesc)};
        ADD_INST_MV(gen, loadInst);
        break; 
    case INT8_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movzbq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, varDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT16_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movzwq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, varDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT32_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movl",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, varDesc, IDX_R32)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT64_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, varDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case FLOAT32:
        {
            uint8_t mmReg = allocateMMRegister(gen, "float_to_sint_temp");
            ADD_INST(gen, {INSTRUCTION, "movss", std::move(src), getCpuRegStr(mmReg, IDX_XMM)});
            ADD_INST(gen, {INSTRUCTION, "cvttss2siq", getCpuRegStr(mmReg, IDX_XMM), generateOperand(gen->cpu, varDesc)});
            freeMMRegister(gen, mmReg);
        }
        break;
    case DOUBLE64:
        {
            uint8_t mmReg = allocateMMRegister(gen, "float_to_sint_temp");
            ADD_INST(gen, {INSTRUCTION, "movsd", std::move(src), getCpuRegStr(mmReg, IDX_XMM)});
            ADD_INST(gen, {INSTRUCTION, "cvttsd2siq", getCpuRegStr(mmReg, IDX_XMM), generateOperand(gen->cpu, varDesc)});
            freeMMRegister(gen, mmReg);
        }
        break;
    }
}

void loadVariableToReg(CodeGenerator *gen, const OpDesc &varDesc, uint8_t targetGr)
{
    if(targetGr == SIGNED_INT_GROUP)
    {
        loadSignedInt(gen, varDesc);
    }
    else if(targetGr == UNSIGNED_INT_GROUP)
    {
        printf("Unsupported group \n");
        exit(-1);
    }
    else if(targetGr == FLOAT_GROUP)
    {
        printf("Unsupported group \n");
        exit(-1);
    }
    else
    {
        printf("Unsupported group \n");
        exit(-1);
    }
    
}

uint8_t allocateRRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i =0; i < 15; i++)
    {
        if(gen->cpu->reg[i].state == REG_FREE)
        {
            gen->cpu->regData[symName] = {
                .storageType = Storage::REG,
                .offset = i
            };
            gen->cpu->reg[i].state = REG_USED;
            gen->cpu->reg[i].symbol = std::move(symName);
            return i;
        }
    }
    printf("No free Registers \n");
    exit(-1);
}

void freeRRegister(CodeGenerator *gen, int index)
{
    if(index < 0)
    {
        return;
    }
    gen->cpu->regData.erase(gen->cpu->reg[index].symbol);
    gen->cpu->reg[index].state = REG_FREE;
    gen->cpu->reg[index].symbol.clear();
}

void freeRRegister(CodeGenerator *gen, const string &symName)
{
    printf("NOT SUPPORTE RREG FREE\n");
    exit(-1);
}

uint8_t allocateMMRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i = XMM0; i <= XMM15; i++)
    {
        if(gen->cpu->reg[i].state == REG_FREE)
        {
            gen->cpu->regData[symName] = {
                .storageType = Storage::REG,
                .offset = i
            };
            gen->cpu->reg[i].state = REG_USED;
            gen->cpu->reg[i].symbol = std::move(symName);
            return i;
        }
    }
    printf("No free MM Registers \n");
    exit(-1);
}

void freeMMRegister(CodeGenerator *gen, int index)
{
    if(index < 0)
    {
        return;
    }
    gen->cpu->regData.erase(gen->cpu->reg[index].symbol);
    gen->cpu->reg[index].state = REG_FREE;
    gen->cpu->reg[index].symbol.clear();
}

void freeMMRegister(CodeGenerator *gen, const string &symName)
{
    printf("NOT SUPPORTE RREG FREE\n");
    exit(-1);
}
