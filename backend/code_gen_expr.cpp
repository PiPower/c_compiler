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
    
    freeRegister(gen, opDesc.operand);
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
    freeRegister(gen, regIdx);
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
    freeRegister(gen, reg);
}

void loadSignedIntToReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc)
{
    string src = generateOperand(gen->cpu, srcDesc);
    Instruction loadInst;
    switch (srcDesc.operandAffi)
    {
    case INT8_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movsbq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT16_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movswq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT32_S:
        if(!registerStores(gen->cpu, RAX, destDesc.operand))
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
        if(!registerStores(gen->cpu, RAX, destDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "movq", "%rax", generateOperand(gen->cpu, destDesc)});
            ADD_INST(gen, {INSTRUCTION, "popq", "%rax"});
        }
        break;
    case INT64_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break; 
    case INT8_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movzbq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT16_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movzwq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT32_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movl",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc, IDX_R32)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT64_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case FLOAT32:
        {
            uint8_t mmReg = allocateMMRegister(gen, "float_to_sint_temp");
            ADD_INST(gen, {INSTRUCTION, "movss", std::move(src), '%' + getCpuRegStr(mmReg, IDX_XMM)});
            ADD_INST(gen, {INSTRUCTION, "cvttss2siq", '%' + getCpuRegStr(mmReg, IDX_XMM), 
                                                            generateOperand(gen->cpu, destDesc)});
            freeRegister(gen, mmReg);
        }
        break;
    case DOUBLE64:
        {
            uint8_t mmReg = allocateMMRegister(gen, "float_to_sint_temp");
            ADD_INST(gen, {INSTRUCTION, "movsd", std::move(src), '%' + getCpuRegStr(mmReg, IDX_XMM)});
            ADD_INST(gen, {INSTRUCTION, "cvttsd2siq", '%' + getCpuRegStr(mmReg, IDX_XMM), 
                                                            generateOperand(gen->cpu, destDesc)});
            freeRegister(gen, mmReg);
        }
        break;
    }
}

void loadUnsignedIntToReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc)
{
    string src = generateOperand(gen->cpu, srcDesc);
    Instruction loadInst;
    switch (srcDesc.operandAffi)
    {
    case INT8_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movsbq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT16_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movswq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT32_S:
        if(!registerStores(gen->cpu, RAX, destDesc.operand))
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
        if(!registerStores(gen->cpu, RAX, destDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "movq", "%rax", generateOperand(gen->cpu, destDesc)});
            ADD_INST(gen, {INSTRUCTION, "popq", "%rax"});
        }
        break;
    case INT64_S:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break; 
    case INT8_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movzbq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT16_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movzwq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT32_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movl",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc, IDX_R32)};
        ADD_INST_MV(gen, loadInst);
        break;
    case INT64_U:
        loadInst = { 
            .type = INSTRUCTION,
            .mnemonic = "movq",
            .src = std::move(src),
            .dest = generateOperand(gen->cpu, destDesc)};
        ADD_INST_MV(gen, loadInst);
        break;
    case FLOAT32:
        printf("Error: double to unsigned conversion is not supported\n");
        exit(-1);
        break;
    case DOUBLE64:
        printf("Error: double to unsigned conversion is not supported\n");
        exit(-1);
        break;
    }
}

void loadFloatToReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc)
{
    string src = generateOperand(gen->cpu, srcDesc);
    OpDesc tmp = srcDesc;
    tmp.operand += generateTmpVarname();
    Instruction loadInst;
    const char* mnemonic = nullptr;
    int regR = -1;
    switch (srcDesc.operandAffi)
    {
    case INT8_S:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movsbl",src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT16_S:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movswl",src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT32_S:
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, src, generateOperand(gen->cpu, destDesc)});
        break;
    case INT64_S:
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssq" : "cvtsi2sdq";
        ADD_INST(gen, {INSTRUCTION, mnemonic, src, generateOperand(gen->cpu, destDesc)});
        break; 
    case INT8_U:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movzbl",src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT16_U:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movzwl",src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT32_U:
        printf("Error: unsigned to float conversion is not supported\n");
        exit(-1);
        break;
    case INT64_U:
        printf("Error: unsigned long to float conversion is not supported\n");
        exit(-1);
        break;
    case FLOAT32:
        if(destDesc.operandAffi == FLOAT32)
        {
            ADD_INST(gen, {INSTRUCTION, "movss", src, generateOperand(gen->cpu, destDesc)});
        }
        else
        {
            ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, destDesc), 
                                                    generateOperand(gen->cpu, destDesc)});
            ADD_INST(gen, {INSTRUCTION, "cvtss2sd", src, generateOperand(gen->cpu, destDesc)});
        }
        break;
    case DOUBLE64:
        if(destDesc.operandAffi == DOUBLE64)
        {
            ADD_INST(gen, {INSTRUCTION, "movsd", src, generateOperand(gen->cpu, destDesc)});
        }
        else
        {
            ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, destDesc), 
                                                    generateOperand(gen->cpu, destDesc)});
            ADD_INST(gen, {INSTRUCTION, "cvtsd2ss", src, generateOperand(gen->cpu, destDesc)});
        }
        break;
    }
    freeRegister(gen, regR);
}

OpDesc writeRegToMem(CodeGenerator *gen, const OpDesc &srcDesc, const std::string &destName)
{
    return OpDesc();
}

void writeSignedIntToMem(CodeGenerator *gen, const OpDesc &varDesc)
{

}

void writeUnsignedIntToMem(CodeGenerator *gen, const OpDesc &varDesc)
{

}

void writeFloatToMem(CodeGenerator *gen, const OpDesc &varDesc, uint16_t operationAffi)
{

}

OpDesc loadVariableToReg(CodeGenerator *gen, const OpDesc &varDesc, uint16_t operationAffi)
{
    if(fetchVariable(gen->cpu, varDesc.operand).storageType == Storage::REG)
    {
        printf("Error: Variable is already in register\n");
        exit(-1);
    }

    OpDesc destDesc = {
        .operandType = OP::VARIABLE,
        .operand = varDesc.operand + generateRegisterName(),
        .operandAffi = operationAffi,
        .scope = gen->localSymtab->scopeLevel
    };
    uint8_t targetGr = getTypeGr(operationAffi);
    if(targetGr == SIGNED_INT_GROUP)
    {
        allocateRRegister(gen, destDesc.operand);
        loadSignedIntToReg(gen, varDesc, destDesc);
    }
    else if(targetGr == UNSIGNED_INT_GROUP)
    {
        allocateRRegister(gen, destDesc.operand);
        loadUnsignedIntToReg(gen, varDesc, destDesc);
    }
    else if(targetGr == FLOAT_GROUP)
    {
        allocateMMRegister(gen, destDesc.operand);
        loadFloatToReg(gen, varDesc, destDesc);
    }
    else
    {
        printf("Unsupported group \n");
        exit(-1);
    }
    
    return destDesc;
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

void freeRegister(CodeGenerator *gen, int index)
{
    if(index < 0)
    {
        return;
    }
    gen->cpu->regData.erase(gen->cpu->reg[index].symbol);
    gen->cpu->reg[index].state = REG_FREE;
}

void freeRegister(CodeGenerator *gen, const std::string &symName)
{
    for(uint8_t i = RAX; i < XMM15; i++)
    {
        if(gen->cpu->reg[i].symbol == symName)
        {
            gen->cpu->regData.erase(gen->cpu->reg[i].symbol);
            gen->cpu->reg[i].state = REG_FREE;
            return;
        }
    }
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
