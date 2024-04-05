#include "../../include/backend/code_generator.hpp"
#include <algorithm>
#define NO_REGISTER -10
const char* registers[8] = {"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};
const char* registersByte[8] = {"%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"};
bool allocatedFlag[8];
char scratchpad[scratchpadSize];
//buffer holds all allocations for global variables
InstructionBuffer globalDefinitions(1000);
SymbolTable symTab;



int generateLabel_ID()
{
    static unsigned int labelCount = 0;
    unsigned int currLabel = labelCount;
    labelCount++;
    return currLabel;
}

using namespace std;

int allocateRegister(int base)
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
    snprintf(scratchpad, scratchpadSize, "\tmovq $%d, %s\n",constant->context.int_32, registers[reg] );
    buffer.writeInstruction(scratchpad);
    return reg;
}

void freeRegister(int reg)
{
    if(reg == NO_REGISTER) {return;}
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

            snprintf(scratchpad, scratchpadSize, "\t%s %s, %s\n", op, registers[r2], registers[r1]  );
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

            snprintf(scratchpad, scratchpadSize, instruction, registers[r2], op, registers[r1]  );
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
            
            snprintf(scratchpad, scratchpadSize, instruction, registers[r1], 
                    resolveOp(root->nodeType) ,registers[r2], sourceReg, registers[r1]);

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
    snprintf(scratchpad, scratchpadSize, instruction, registers[r2], registers[r1], comp, registersByte[r1], registers[r1]);
    buffer.writeInstruction(scratchpad);
    freeRegister(r2);

    return r1;
}

static int translateLogicalAND(InstructionBuffer& buffer, AstNode* root)
{
    static const char* tail = "\tmovq $1, %s\n"
                              "\tjmp joinLabel%d\n"
                              "falseLabel%d:\n"
                              "\tmovq $0, %s\n"
                              "joinLabel%d:\n";

    unsigned int falseLabel = generateLabel_ID();
    unsigned int joinLabel = generateLabel_ID();
    int targetRegister = allocateRegister();
    for(AstNode* child : root->children)
    {
        int r = translate(buffer, child);
        sprintf(scratchpad, "\tcmpq  $0, %s\n" "\tje falseLabel%d\n", registers[r], falseLabel);
        freeRegister(r);
        buffer.writeInstruction(scratchpad);
    }

    sprintf(scratchpad, tail,  registers[targetRegister], joinLabel, falseLabel, registers[targetRegister], joinLabel);
    buffer.writeInstruction(scratchpad);
    return targetRegister;
}

static int translateLogicalOR(InstructionBuffer& buffer, AstNode* root)
{
        static const char* tail = "\tmovq $0, %s\n"
                                  "\tjmp joinLabel%d\n"
                                  "trueLabel%d:\n"
                                  "\tmovq $1, %s\n"
                                  "joinLabel%d:\n";

    unsigned int trueLabel = generateLabel_ID();
    unsigned int joinLabel = generateLabel_ID();
    int targetRegister = allocateRegister();
    for(AstNode* child : root->children)
    {
        int r = translate(buffer, child);
        sprintf(scratchpad, "\tcmpq  $0, %s\n" "\tjne trueLabel%d\n", registers[r], trueLabel);
        freeRegister(r);
        buffer.writeInstruction(scratchpad);
    }

    sprintf(scratchpad, tail,  registers[targetRegister], joinLabel, trueLabel, registers[targetRegister], joinLabel);
    buffer.writeInstruction(scratchpad);
    return targetRegister;
}

int load64Identifier(InstructionBuffer& buffer, AstNode* root)
{

    SymbolEntry& entry = symTab[STRING(root)];
    if(entry.x == UNDEFINED_ENTRY)
    {
        fprintf(stdout, "trying to refrence undefined variable\n");
        exit(-1);
    }

    int r = allocateRegister();
    snprintf(scratchpad, scratchpadSize, "\tmovq %s(%%rip), %s\n", STRING(root).c_str(), registers[r]);
    buffer.writeInstruction(scratchpad);
    return r;
}


int translateIfStatement(InstructionBuffer& buffer, AstNode* root)
{
    int escapeLabel = generateLabel_ID();

    AstNode* currentIfNode = root;
    while (currentIfNode != nullptr &&currentIfNode->nodeType == NodeType::IF)
    {
        int falseJumpLabel = generateLabel_ID();
        int r_cond = translate(buffer, currentIfNode->children[0]);

        snprintf(scratchpad, scratchpadSize, "\tcmpq $0, %s\n""\tje false%d\n", registers[r_cond], falseJumpLabel);
        buffer.writeInstruction(scratchpad);
        freeRegister(r_cond);

        if(currentIfNode->children[1]!= nullptr)
        {
            int r = translate(buffer, currentIfNode->children[1]);
            freeRegister(r);
        }
        snprintf(scratchpad, scratchpadSize, "\tjmp escapeLabel%d\n", escapeLabel);
        buffer.writeInstruction(scratchpad);

        snprintf(scratchpad, scratchpadSize, "false%d:\n", falseJumpLabel);
        buffer.writeInstruction(scratchpad);
        if(currentIfNode->children.size() == 3)
        {
            currentIfNode = currentIfNode->children[2];
        }
        else
        {
            break;
        }

    }
    if(currentIfNode != nullptr  && currentIfNode->nodeType != NodeType::IF)
    {
        int r = translate(buffer, currentIfNode);
        freeRegister(r);
    }
    snprintf(scratchpad, scratchpadSize, "escapeLabel%d:\n", escapeLabel);
    buffer.writeInstruction(scratchpad);
    return NO_REGISTER;
}

int translateAssignment(InstructionBuffer& buffer, AstNode* root)
{
    int reg = translate(buffer, root->children[1]);
    static const char* assignmentInstruction = "\tmovq %s, %s(%rip)\n";
    AstNode* identifier = root->children[0];

    snprintf(scratchpad, scratchpadSize, assignmentInstruction, registers[reg], STRING(identifier).c_str());
    buffer.writeInstruction(scratchpad);
    return reg;
}

int translateWhileLoop(InstructionBuffer& buffer, AstNode* root)
{
    int startLabel = generateLabel_ID();
    int postLoopLabel = generateLabel_ID();

    snprintf(scratchpad, scratchpadSize, "loopStart%d:\n", startLabel);
    buffer.writeInstruction(scratchpad);
    int reg = translate(buffer, root->children[0]);

    snprintf(scratchpad, scratchpadSize, "\tcmpq $0, %s\n" "\tje postLoopLabel%d\n", registers[reg], postLoopLabel);
    buffer.writeInstruction(scratchpad);
    freeRegister(reg);

    freeRegister(translate( buffer, root->children[1]));
    snprintf(scratchpad, scratchpadSize, "\tjmp loopStart%d\n" "postLoopLabel%d:\n", startLabel, postLoopLabel);
    buffer.writeInstruction(scratchpad);

    return NO_REGISTER;
}
int translateDoWhileLoop(InstructionBuffer& buffer, AstNode* root)
{
    int startLabel = generateLabel_ID();

    snprintf(scratchpad, scratchpadSize, "loopStart%d:\n", startLabel);
    buffer.writeInstruction(scratchpad);

    freeRegister(translate( buffer, root->children[1]));

    int reg = translate(buffer, root->children[0]);
    snprintf(scratchpad, scratchpadSize, "\tcmpq $0, %s\n" "\tjne loopStart%d\n", registers[reg], startLabel);
    buffer.writeInstruction(scratchpad);
    freeRegister(reg);

    return NO_REGISTER;
}

int translateForLoop(InstructionBuffer& buffer, AstNode* root)
{
    int startLabel = generateLabel_ID();
    int loopEnd = generateLabel_ID(); 
    freeRegister(translate(buffer, root->children[0]));

    snprintf(scratchpad, scratchpadSize, "loopStart%d:\n", startLabel);
    buffer.writeInstruction(scratchpad);
    if(root->children[1] != nullptr)
    {
        int reg = translate(buffer, root->children[1]);
        snprintf(scratchpad, scratchpadSize, "\tcmpq $0, %s\n" "\tje loopEnd%d\n", registers[reg], loopEnd);
        buffer.writeInstruction(scratchpad);
        freeRegister(reg);
    }

    freeRegister(translate(buffer, root->children[3]));
    freeRegister(translate(buffer, root->children[2]));

    snprintf(scratchpad, scratchpadSize,  "\tjmp loopStart%d\n""loopEnd%d:\n",startLabel, loopEnd);
    buffer.writeInstruction(scratchpad);
    return NO_REGISTER;
}

int translateFunction(InstructionBuffer& buffer, AstNode* root)
{
    AstNode* identifier = root->children[0];


}
//returns which register to us
int translate(InstructionBuffer& buffer, AstNode* root)
{
    if(root == nullptr) {return NO_REGISTER;}
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
    case NodeType::ASSIGNMENT:
    case NodeType::ADD_ASSIGNMENT:
    case NodeType::SUBB_ASSIGNMENT:
    case NodeType::MUL_ASSIGNMENT:
    case NodeType::DIV_ASSIGNMENT:
    case NodeType::MOD_ASSIGNMENT:
    case NodeType::EXC_OR_ASSIGNMENT:
    case NodeType::L_SHIFT_ASSIGNMENT:
    case NodeType::R_SHIFT_ASSIGNMENT:
    case NodeType::AND_ASSIGNMENT:
    case NodeType::OR_ASSIGNMENT:
        return translateAssignment(buffer, root);
    case NodeType::CONSTANT:
        return load64Register(buffer, root);
    case NodeType::LOG_AND:
        return translateLogicalAND(buffer, root);
     case NodeType::LOG_OR:
        return translateLogicalOR(buffer, root);
    case NodeType::DECLARATION:
        return translateDeclaration(buffer, root);
    case NodeType::IDENTIFIER:
        return load64Identifier(buffer, root);
    case NodeType::IF:
        return translateIfStatement(buffer, root);
    case NodeType::WHILE_LOOP:
        return translateWhileLoop(buffer, root);
    case NodeType::DO_WHILE_LOOP:
        return translateDoWhileLoop(buffer, root);
    case NodeType::FOR_LOOP:
        return translateForLoop(buffer, root);
    case NodeType::BLOCK:
    {
        for(AstNode* instruction : root->children)
        {
            int r = translate(buffer, instruction);
            freeRegister(r);
        }
    } return NO_REGISTER;
    default:
        fprintf(stdout, "unsupported node type by codegen\n");
        exit(-1);
    }

    return -1;
}

InstructionBuffer generateCode(std::vector<AstNode*>& instructionSequence)
{
    InstructionBuffer buffer(10000);
    for(AstNode* root : instructionSequence)
    {
        int reg = translate(buffer, root);
        freeRegister(reg);
    }
    globalDefinitions.writeInstruction(buffer);
    return globalDefinitions;
}