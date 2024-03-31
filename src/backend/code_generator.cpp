#include "../../include/backend/code_generator.hpp"

const char* registers[8] = {"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};
const char* registersByte[8] = {"%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"};
bool allocatedFlag[8];
static char scratchpad[500];

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

static string resolveOp(NodeType& type)
{
    switch (type)
    {
    case NodeType::OR:
        return "orq";
    case NodeType::SUBTRACT:
        return "subq";
    case NodeType::ADD:
        return "addq";
    case NodeType::MULTIPLY:
        return "imulq";
    case NodeType::L_SHIFT:
        return "shlq";
    case NodeType::R_SHIFT:
        return "shrq";
    case NodeType::AND:
        return  "andq";
    case NodeType::EXC_OR:
        return  "xorq";
    }
    fprintf(stdout, "Unsoported op type\n");
    exit(-1);
    return "";
}

static string resolveComparison(NodeType& type)
{
    switch (type)
    {
    case NodeType::EQUAL:
        return "sete";
    case NodeType::NOT_EQUAL:
        return "setne";
    case NodeType::GREATER:
        return "setg";
    case NodeType::GREATER_EQUAL:
        return "setge";
    case NodeType::LESS:
        return "setl";
    case NodeType::LESS_EQUAL:
        return "setle";
    }
    fprintf(stdout, "Unsoported comparison type\n");
    exit(-1);
    return "";
}


int load64Register(InstructionBuffer& buffer, AstNode* constant)
{
    int reg = allocateRegister();
    snprintf(scratchpad, 500, "\tmovq $%d, %s\n",constant->context.int_32, registers[reg] );
    buffer.writeInstruction(scratchpad);
    return reg;
}

void freeRegister(int reg)
{
    allocatedFlag[reg] = false;
}

static int translateBinaryExpression(InstructionBuffer& buffer, AstNode* root)
{

    int r1 = translate(buffer, root->children[0]);
    int r2 = translate(buffer, root->children[1]);

    switch (root->nodeType)
    {
    case NodeType::OR:
    case NodeType::AND:
    case NodeType::EXC_OR:
    case NodeType::ADD:
    case NodeType::SUBTRACT:
    case NodeType::MULTIPLY:
        {
            string op = resolveOp(root->nodeType);

            snprintf(scratchpad, 500, "\t%s %s, %s\n", op.c_str(), registers[r2], registers[r1]  );
            buffer.writeInstruction(scratchpad);
            freeRegister(r2);
            return r1;
        }
        break;
    case NodeType::L_SHIFT:
    case NodeType::R_SHIFT:
        {
            string op = resolveOp(root->nodeType);
            const char* instruction = "\txorq %rcx, %rcx\n"
                                      "\tmovq %s, %rcx\n"
                                      "\t%s %%cl, %s\n";

            snprintf(scratchpad, 500, instruction, registers[r2], op.c_str(), registers[r1]  );
            buffer.writeInstruction(scratchpad);
            freeRegister(r2);
            return r1;
        }
        break;
    case NodeType::DIVIDE:
        {
            const char* instruction = "\tmovq %s, %rax\n"
                                      "\txorq %rdx, %rdx\n" 
                                      "\tidivq %s\n" 
                                      "\tmovq %rax, %s\n";
            
            snprintf(scratchpad, 500, instruction, registers[r1], registers[r2], registers[r1]);

            buffer.writeInstruction(scratchpad);
            freeRegister(r2);
            return r1;
        }
        break;
    }

}

static int translateComparison(InstructionBuffer& buffer, AstNode* root)
{
    int r1 = translate(buffer, root->children[0]);
    int r2 = translate(buffer, root->children[1]);

    const char* instruction = "\tcmpq %s, %s\n"
                              "\t%s %s\n"
                              "\tandq $255, %s\n";
    string comp = resolveComparison(root->nodeType);
    snprintf(scratchpad, 500, instruction, registers[r2], registers[r1], comp.c_str(), registersByte[r1], registers[r1]);
    buffer.writeInstruction(scratchpad);
    freeRegister(r2);


}
//returns which register to use
int translate(InstructionBuffer& buffer, AstNode* root)
{
    switch (root->nodeType)
    {
    case NodeType::OR:
    case NodeType::AND:
    case NodeType::EXC_OR:
    case NodeType::ADD:
    case NodeType::SUBTRACT:
    case NodeType::MULTIPLY:
    case NodeType::L_SHIFT:
    case NodeType::R_SHIFT:
    case NodeType::DIVIDE:
        return translateBinaryExpression(buffer, root);
    case NodeType::EQUAL:
    case NodeType::NOT_EQUAL:
    case NodeType::GREATER:
    case NodeType::GREATER_EQUAL:
    case NodeType::LESS:
    case NodeType::LESS_EQUAL:
        return translateComparison(buffer, root);
    case NodeType::CONSTANT:
        return load64Register(buffer, root);
    default:
        fprintf(stdout, "unsupported node type by codegen\n");
        exit(-1);
    }

    return -1;
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