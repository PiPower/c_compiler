#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
using namespace std;

void translateExpr(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::MINUS:
        translateNegation(gen, parseTree);
        break;
    }
}

void translateNegation(CodeGenerator *gen, AstNode *parseTree)
{
    CLEAR_OP(gen);
    dispatch(gen, parseTree->children[0]);
    if(gen->opDesc.op == OP::CONSTANT)
    {
        gen->opDesc.operand.insert(0, 1, '-');
    }
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
        auto varDescIter = gen->cpu->data.find(destDesc.operand);
        const VariableDesc* desc = &varDescIter->second;
        if( (uint64_t)(value >> 32 ) > 0)
        {
            if(desc->storageType != Storage::REG)
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

    inst.dest = genAssignmentDest(gen->cpu, destDesc);
    ADD_INST_MV(gen, inst);
    freeRRegister(gen, regIdx);
    gen->opDesc = { .op = OP::NONE };
}

void moveConstantFloat(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc)
{
    int reg = -1;
    OpDesc tempDesc = { .op = OP::NONE};
    if(gen->cpu->data[destDesc.operand].storageType != Storage::REG)
    {
        tempDesc = {
            .op = OP::TEMP_VAR,
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
    if(tempDesc.op == OP::NONE)
    {
        movToReg.dest = genAssignmentDest(gen->cpu, destDesc);
    }
    else
    {
        movToReg.dest = genAssignmentDest(gen->cpu, tempDesc);
    }

    ADD_INST_MV(gen, movToReg);
    if(tempDesc.op != OP::NONE)
    {
        const Instruction& prev = gen->code.back();
        Instruction moveToTarget = {
            .type = INSTRUCTION,
            .mnemonic = prev.mnemonic,
            .src = prev.dest,
            .dest = genAssignmentDest(gen->cpu, destDesc)
        };
        ADD_INST_MV(gen, moveToTarget);
    }
    freeMMRegister(gen, reg);
    gen->opDesc = {};
}

uint8_t allocateRRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i =0; i < 15; i++)
    {
        if(gen->cpu->reg[i].state == REG_FREE)
        {
            gen->cpu->data[symName] = {
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
    gen->cpu->data.erase(gen->cpu->reg[index].symbol);
    gen->cpu->reg[index].state = REG_FREE;
    gen->cpu->reg[index].symbol.clear();
}

uint8_t allocateMMRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i = XMM0; i <= XMM15; i++)
    {
        if(gen->cpu->reg[i].state == REG_FREE)
        {
            gen->cpu->data[symName] = {
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
    gen->cpu->data.erase(gen->cpu->reg[index].symbol);
    gen->cpu->reg[index].state = REG_FREE;
    gen->cpu->reg[index].symbol.clear();
}
