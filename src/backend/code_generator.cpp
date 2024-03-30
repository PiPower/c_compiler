#include "../../include/backend/code_generator.hpp"

const char* registers[16] = { "rax",  "rbx","rcx","rdx","rsi","rdi","rbp","rsp",
                                "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
bool allocatedFlag[16];

using namespace std;

int allocateRegister()
{
    for(int i=0 ; i < 16; i++)
    {
        if(!allocatedFlag[i])
        {
            allocatedFlag[i] = true;
            return i;
        }
    }
    fprintf(stdout, "no registers to use \n");
    exit(-1);
    return -1;
}

int load64Register(InstructionBuffer& buffer, AstNode* constant)
{
    int reg = allocateRegister();
    string instruction = "movq $" + to_string(constant->context.int_32) + ", %"  + registers[reg] + " \n";
    buffer.writeInstruction(instruction.c_str());
    return reg;
}
//returns which register to use
int translate(InstructionBuffer& buffer, AstNode* root)
{
    switch (root->nodeType)
    {
    case NodeType::MULTIPLY:
        {
            int r1 = translate(buffer, root->children[0]);
            int r2 = translate(buffer, root->children[1]);

            string instruction = string{"imulq %"} + registers[r1] + ", %"  + registers[r2] + " \n";
            buffer.writeInstruction(instruction.c_str());
            return r2;
        }
        break;
    case NodeType::CONSTANT:
        return load64Register(buffer, root);
    default:
        break;
    }
}

InstructionBuffer generateCode(std::vector<AstNode*>& instructionSequence)
{
    InstructionBuffer buffer(10000);
    buffer.writeInstruction(".globl main \n");
    buffer.writeInstruction(".text \n");
    buffer.writeInstruction("main: \n");
    for(AstNode* root : instructionSequence)
    {
        translate(buffer, root);
    }
    return buffer;
}