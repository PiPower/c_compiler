#include "../../include/backend/code_generator.hpp"
#include <algorithm>
const char* registers[8] = {"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};
const char* registersByte[8] = {"%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"};
const char* functionRegisters[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
bool allocatedFlag[8];
//buffer holds all allocations for global variables
InstructionBuffer globalDefinitions(1000);
CompilationState compilationState;

using namespace std;

int generateLabel_ID()
{
    static unsigned int labelCount = 0;
    unsigned int currLabel = labelCount;
    labelCount++;
    return currLabel;
}

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



int load64Register(InstructionBuffer& buffer, AstNode* constant)
{
    int reg = allocateRegister();
    bprintf(&buffer, "\tmovq $%ld, %s\n",constant->context.int_32, registers[reg]);
    return reg;
}


void freeRegister(int reg)
{
    if(reg == NO_REGISTER) {return;}
    allocatedFlag[reg] = false;
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
            bprintf(&buffer, "\t%s %s, %s\n", op, registers[r2], registers[r1] );
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
            bprintf(&buffer,  instruction, registers[r2], op, registers[r1]  );
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
            bprintf(&buffer, instruction, registers[r1], 
                    resolveOp(root->nodeType) ,registers[r2], sourceReg, registers[r1]);
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
    bprintf(&buffer, instruction, registers[r2], registers[r1], comp, registersByte[r1], registers[r1]);
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
        bprintf(&buffer, "\tcmpq  $0, %s\n" "\tje falseLabel%d\n", registers[r], falseLabel );
        freeRegister(r);
    }

    bprintf(&buffer, tail,  registers[targetRegister], joinLabel, falseLabel, registers[targetRegister], joinLabel);
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
        bprintf(&buffer, "\tcmpq  $0, %s\n" "\tjne trueLabel%d\n", registers[r], trueLabel);
        freeRegister(r);
    }

    bprintf(&buffer, tail,  registers[targetRegister], joinLabel, trueLabel, registers[targetRegister], joinLabel);
    return targetRegister;
}


int translateIfStatement(InstructionBuffer& buffer, AstNode* root)
{
    int escapeLabel = generateLabel_ID();

    AstNode* currentIfNode = root;
    while (currentIfNode != nullptr &&currentIfNode->nodeType == NodeType::IF)
    {
        int falseJumpLabel = generateLabel_ID();
        int r_cond = translate(buffer, currentIfNode->children[0]);

        bprintf(&buffer, "\tcmpq $0, %s\n""\tje false%d\n", registers[r_cond], falseJumpLabel);
        freeRegister(r_cond);

        if(currentIfNode->children[1]!= nullptr)
        {
            int r = translate(buffer, currentIfNode->children[1]);
            freeRegister(r);
        }
        bprintf(&buffer,  "\tjmp escapeLabel%d\nfalse%d:\n", escapeLabel, falseJumpLabel);
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
    bprintf(&buffer,"escapeLabel%d:\n", escapeLabel);
    return NO_REGISTER;
}

int translateWhileLoop(InstructionBuffer& buffer, AstNode* root)
{
    int startLabel = generateLabel_ID();
    int postLoopLabel = generateLabel_ID();



    bprintf(&buffer, "loopStart%d:\n", startLabel);
    int reg = translate(buffer, root->children[0]);

    bprintf(&buffer,"\tcmpq $0, %s\n" "\tje postLoopLabel%d\n", registers[reg], postLoopLabel);
    freeRegister(reg);

    freeRegister(translate( buffer, root->children[1]));
    bprintf(&buffer, "\tjmp loopStart%d\n" "postLoopLabel%d:\n", startLabel, postLoopLabel);
    return NO_REGISTER;
}
int translateDoWhileLoop(InstructionBuffer& buffer, AstNode* root)
{
    int startLabel = generateLabel_ID();

    bprintf(&buffer, "loopStart%d:\n", startLabel);

    freeRegister(translate( buffer, root->children[1]));

    int reg = translate(buffer, root->children[0]);
    bprintf(&buffer, "\tcmpq $0, %s\n" "\tjne loopStart%d\n", registers[reg], startLabel);
    freeRegister(reg);

    return NO_REGISTER;
}

int translateForLoop(InstructionBuffer& buffer, AstNode* root)
{
    int startLabel = generateLabel_ID();
    int loopEnd = generateLabel_ID(); 
    freeRegister(translate(buffer, root->children[0]));

    bprintf(&buffer, "loopStart%d:\n", startLabel);
    if(root->children[1] != nullptr)
    {
        int reg = translate(buffer, root->children[1]);
        bprintf(&buffer, "\tcmpq $0, %s\n" "\tje loopEnd%d\n", registers[reg], loopEnd);
        freeRegister(reg);
    }

    freeRegister(translate(buffer, root->children[3]));
    freeRegister(translate(buffer, root->children[2]));
    bprintf(&buffer, "\tjmp loopStart%d\n""loopEnd%d:\n",startLabel, loopEnd);
    
    return NO_REGISTER;
}

int translateFunctionCall(InstructionBuffer& buffer, AstNode* functionData)
{
    bool stateR8 = allocatedFlag[0];
    bool stateR9 = allocatedFlag[1];
    if(stateR8) {buffer.writeInstruction("\tpushq %r8\n"); allocatedFlag[0] = false;}
    if(stateR9) {buffer.writeInstruction("\tpushq %r9\n");  allocatedFlag[1] = false;}


    AstNode* args = functionData->children[1];
    AstNode* functionName = functionData->children[0];
    int arg_count =  args->children.size();
    int pushedBytes = 0;
    for(int j = args->children.size(); j > 6; j--)
    {
        int r = translate(buffer, args->children[j - 1]);
        bprintf(&buffer, "\tpushq %s\n", registers[r]);
        pushedBytes+=8;
        freeRegister(r);
    }

    int i = 1;
    while (i <= 6  && i < args->children.size())
    {
        int r = translate(buffer, args->children[i - 1]);
        bprintf(&buffer, "\tmovq %s, %s\n", registers[r], functionRegisters[i - 1]);
        if(i == 5 )
        {
            allocatedFlag[0] = true;
            if(r == 0 ){i++; continue;}
        }
        if(i == 6 )
        {
            allocatedFlag[1] = true;
            if(r == 1 ){i++; continue;}
        }
        freeRegister(r);
        i++;
    }
    
    bprintf(&buffer, "\tcall %s", STRING(functionName).c_str());
    buffer.bufferForLateBinding(5);
    if(pushedBytes !=0)
    {
        bprintf(&buffer, "\taddq $%d, %rsp\n", pushedBytes);
    }

    if(stateR9) {buffer.writeInstruction("\tpopq %r9\n"); }
    if(stateR8) {buffer.writeInstruction("\tpopq %r8\n");}

    allocatedFlag[0] = stateR8;
    allocatedFlag[1] = stateR9;
    return NO_REGISTER;
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
        return loadIdentifier(buffer, root);
    case NodeType::IF:
        return translateIfStatement(buffer, root);
    case NodeType::WHILE_LOOP:
        return translateWhileLoop(buffer, root);
    case NodeType::DO_WHILE_LOOP:
        return translateDoWhileLoop(buffer, root);
    case NodeType::FOR_LOOP:
        return translateForLoop(buffer, root);
    case NodeType::FUNCTION_CALL:
        return translateFunctionCall(buffer, root);
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


    resolveFunctionBinding(buffer);
    return buffer;
}