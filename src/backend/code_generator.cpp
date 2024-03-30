#include "../../include/backend/code_generator.hpp"

const char* registers[8] = {"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};
bool allocatedFlag[8];
static char dataBuffer[500];

using namespace std;

int allocateRegister(int base = 0 )
{
    for(int i=base ; i < 8; i++)
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
    snprintf(dataBuffer, 500, "\tmovq $%d, %s\n",constant->context.int_32, registers[reg] );
    buffer.writeInstruction(dataBuffer);
    return reg;
}

void freeRegister(int reg)
{
    allocatedFlag[reg] = false;
}
//returns which register to use
int translate(InstructionBuffer& buffer, AstNode* root)
{
    switch (root->nodeType)
    {
    case NodeType::ADD:
    case NodeType::SUBTRACT:
    case NodeType::MULTIPLY:
        {
            int r1 = translate(buffer, root->children[0]);
            int r2 = translate(buffer, root->children[1]);
            string op;

            switch (root->nodeType)
            {
            case NodeType::SUBTRACT:
                op = "sub";
                break;
            case NodeType::ADD:
                op = "add";
                break;
            case NodeType::MULTIPLY:
                op = "imulq";
                break;
            
            default:
                break;
            }

            snprintf(dataBuffer, 500, "\t%s %s, %s\n", op.c_str(), registers[r2], registers[r1]  );
            buffer.writeInstruction(dataBuffer);
            freeRegister(r2);
            return r1;
        }
        break;
    case NodeType::L_SHIFT:
    case NodeType::R_SHIFT:
        {
            int r1 = translate(buffer, root->children[0]);
            int r2 = translate(buffer, root->children[1]);
            string op;
            switch (root->nodeType)
            {
            case NodeType::L_SHIFT:
                op = "shlq";
                break;
            case NodeType::R_SHIFT:
                op = "shrq";
                break;
            }
            const char* instruction = "\txorq %rcx, %rcx\n"
                                      "\tmovq %s, %rcx\n"
                                      "\t%s %%cl, %s\n";
            snprintf(dataBuffer, 500, instruction, registers[r2], op.c_str(), registers[r1]  );
            buffer.writeInstruction(dataBuffer);
            freeRegister(r2);
            return r1;
        }
        break;
    case NodeType::DIVIDE:
        {
            int r1 = translate(buffer, root->children[0]);
            int r2 = translate(buffer, root->children[1]);
            const char* instruction = "\tmovq %s, %rax\n"
                                      "\txorq %rdx, %rdx\n" 
                                      "\tidivq %s\n" 
                                      "\tmovq %rax, %s\n";
            
            snprintf(dataBuffer, 500, instruction, registers[r1], registers[r2], registers[r1]);

            buffer.writeInstruction(dataBuffer);
            freeRegister(r2);
            return r1;
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