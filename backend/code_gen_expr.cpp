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
    long int value = encodeAsBinary(constant);
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

    if(destDesc.scope > 0)
    {
        switch( gen->cpu->data[destDesc.operand].storageType)
        {
        case Storage::REG:
            printf("Internal Error: storage type error \n");
            exit(-1);
            break;
        case Storage::STACK:
            inst.dest += to_string(gen->cpu->data[destDesc.operand].offset) +
                        '('  + '%'+ cpu_registers_str[RBP][0] + ')';
            break;
        default:
            printf("Internal Error: storage type error \n");
            exit(-1);
            break;
        }
    }
    else
    {

    }
    ADD_INST_MV(gen, inst);
    freeRRegister(gen, regIdx);
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
