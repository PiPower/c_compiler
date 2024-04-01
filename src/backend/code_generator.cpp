#include "../../include/backend/code_generator.hpp"
#include <algorithm>
const char* registers[8] = {"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};
const char* registersByte[8] = {"%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"};
bool allocatedFlag[8];
static char scratchpad[500];

SymbolTable symTab;



int generateLabel_ID()
{
    static unsigned int labelCount = 0;
    unsigned int currLabel = labelCount;
    labelCount++;
    return currLabel;
}

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

static const char* resolveOp(NodeType& type)
{
    static const char* intructionTab[] = {"imulq", "idivq", "idivq", "addq", "subq", "shlq", "shrq", "andq", "xorq", "orq"};
    int tab_index = (int)type - (int)NodeType::MULTIPLY;

    if(tab_index > (int)NodeType::OR - (int)NodeType::MULTIPLY)
    {
        fprintf(stdout, "Unsoported op type\n");
        exit(-1);
    }
    return intructionTab[tab_index];
}


static const char* resolveComparison(NodeType& type, bool jump = false)
{
    const char* compOpsSet[] = {"setl", "setg", "setle", "setge", "sete", "setne" };
    const char* compOpsJump[] = {"jl", "jg", "jle", "jge", "je", "jne" };
    int tab_index = (int)type - (int)NodeType::LESS;

    if(tab_index > (int)NodeType::NOT_EQUAL - (int)NodeType::LESS)
    {
        fprintf(stdout, "Unsoported comparison type\n");
        exit(-1);
    }
    return jump ? compOpsJump[tab_index] : compOpsSet[tab_index];
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
            const char*  op = resolveOp(root->nodeType);

            snprintf(scratchpad, 500, "\t%s %s, %s\n", op, registers[r2], registers[r1]  );
            buffer.writeInstruction(scratchpad);
            freeRegister(r2);
            return r1;
        }
        break;
    case NodeType::L_SHIFT:
    case NodeType::R_SHIFT:
        {
            const char* const op = resolveOp(root->nodeType);
            const char* instruction = "\txorq %rcx, %rcx\n"
                                      "\tmovq %s, %rcx\n"
                                      "\t%s %%cl, %s\n";

            snprintf(scratchpad, 500, instruction, registers[r2], op, registers[r1]  );
            buffer.writeInstruction(scratchpad);
            freeRegister(r2);
            return r1;
        }
        break;
    case NodeType::DIVIDE_MODULO:
    case NodeType::DIVIDE:
        {
            const char* sourceReg = root->nodeType == NodeType::DIVIDE ? "%rax" : "%rdx";
            const char* instruction = "\tmovq %s, %rax\n"
                                      "\txorq %rdx, %rdx\n" 
                                      "\t%s %s\n" 
                                      "\tmovq %s, %s\n";
            
            snprintf(scratchpad, 500, instruction, registers[r1],resolveOp(root->nodeType) ,registers[r2], sourceReg, registers[r1]);

            buffer.writeInstruction(scratchpad);
            freeRegister(r2);
            return r1;
        }
        break;
    }
    return -1;
}

static int translateComparison(InstructionBuffer& buffer, AstNode* root)
{
    int r1 = translate(buffer, root->children[0]);
    int r2 = translate(buffer, root->children[1]);

    const char* instruction = "\tcmpq %s, %s\n"
                              "\t%s %s\n"
                              "\tandq $255, %s\n";
    const char* comp = resolveComparison(root->nodeType);
    snprintf(scratchpad, 500, instruction, registers[r2], registers[r1], comp, registersByte[r1], registers[r1]);
    buffer.writeInstruction(scratchpad);
    freeRegister(r2);

    return r1;
}

static int translateLogicalOps(InstructionBuffer& buffer, AstNode* root)
{
    int r1 = translate(buffer, root->children[0]);
    int r2 = translate(buffer, root->children[1]);
    fprintf(stdout, "logical ops are currently unspported\n");
    exit(-1);
    return 0;
}

static int translateDeclaration(InstructionBuffer& buffer, AstNode* root)
{
    buffer.writeInstruction("\t.data\n");
    static const char* defTemplate = "\t.globl %s\n"
                                     "%s:\n"
                                     "\t.zero 8\n";

    InstructionBuffer definitionBuffer(1000);
    for(AstNode* declaration : root->children)
    {
        AstNode* identifier = declaration->children[0];
        SymbolEntry& entry = symTab[STRING(identifier)];

        if(entry.x != -1)
        {
            fprintf(stdout, "redeclaration of global variable\n");
            exit(-1);
        }

        entry.x = 0;
        int  x =2;
        snprintf(scratchpad, 500, defTemplate, STRING(identifier).c_str(), STRING(identifier).c_str());
        buffer.writeInstruction(scratchpad);
        if(declaration->children.size() > 1)
        {
            int reg = translate(definitionBuffer, declaration->children[1]);
            snprintf(scratchpad, 500, "\tmovq %s, %s(%%rip)\n", registers[reg], STRING(identifier).c_str());
            definitionBuffer.writeInstruction(scratchpad);
        }


    }
    buffer.writeInstruction("\t.text\n");
    buffer.writeInstruction(definitionBuffer);
    return 0;
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
    case NodeType::LOG_AND:
    case NodeType::LOG_OR:
        return translateLogicalOps(buffer, root);
    case NodeType::DECLARATION:
        return translateDeclaration(buffer, root);
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