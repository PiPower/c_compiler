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

void generateCodeForU_SICA(CodeGenerator *gen, const std::string &constant, const OpDesc &destDesc)
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
        inst.mnemonic = "movq";
        if( (uint64_t)(value >> 32 ) > 0)
        {
            regIdx = (int)allocateRRegister(gen, "init_tmp");
            Instruction instLocal = {INSTRUCTION, "movabsq", 
                                encodeIntAsString(value, 8), 
                                cpu_registers_str[regIdx][0] };
            instLocal.dest = '%' + instLocal.dest;
            inst.src = instLocal.dest;
            ADD_INST_MV(gen, instLocal);
        }
        else
        {
            inst.src = encodeIntAsString(value, 8);
        }
    }

    inst.dest = genAssignmentDest(gen->cpu, destDesc);
    ADD_INST_MV(gen, inst);
    freeRRegister(gen, regIdx);
}

void generateCodeForSf_DfCA(CodeGenerator* gen, const std::string& constant,  const OpDesc &destDesc)
{
    string label;
    uint64_t val = 0;
    Instruction movToReg = {
        .type = INSTRUCTION
    }; 
     Instruction movToVar = {
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

    uint8_t reg = allocateMMRegister(gen, "init_float_tmp");
    movToReg.src = label + "(%rip)";
    movToReg.dest = cpu_registers_str[reg][IDX_XMM];
    movToReg.dest = '%' + movToReg.dest;

    movToVar.src = movToReg.dest ;
    movToVar.mnemonic = movToReg.mnemonic;
    movToVar.dest = genAssignmentDest(gen->cpu, destDesc);

    ADD_INST_MV(gen, movToReg);
    ADD_INST_MV(gen, movToVar);
    freeMMRegister(gen, reg);
}

uint8_t allocateRRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i =0; i < 15; i++)
    {
        if(gen->cpu->reg[i].state == REG_FREE)
        {
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
    gen->cpu->reg[index].state = REG_FREE;
    gen->cpu->reg[index].symbol.clear();
}

uint8_t allocateMMRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i = XMM0; i <= XMM15; i++)
    {
        if(gen->cpu->reg[i].state == REG_FREE)
        {
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
    gen->cpu->reg[index].state = REG_FREE;
    gen->cpu->reg[index].symbol.clear();
}
