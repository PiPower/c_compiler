#ifndef CODE_GEN
#define CODE_GEN
#include "../frontend/parser.hpp"
/*
    Mnemomic can either be instruction or label.
    If it is a label then src is a sequence of
    directives used  before writing a label and 
    dest is a sequence of directives used after
    writing a label.
    If mnemonic is instruction then src is source operand
    and dest is destination operand

*/

#define LABEL 0x01
#define INSTRUCTION 0x02

struct Instruction
{
    uint8_t type;
    std::string mnemonic;
    std::string src;
    std::string dest;
};

typedef std::vector<Instruction> InstructionSeq;
struct CpuState;

/*
localSymtab - symboltable of currently translated block;
symtab - global symboltable ;
allocator - ast nodes allocator;
code - sequence of translated instructions in x86-64 gnu asm;
operand - used to pass translation context between calls;
cpu - pointer to structure representing cpu;
*/

enum class OP
{
    NONE,
    CONSTANT,
    VARIABLE,
    TEMP_VAR,
};

struct OpDesc
{
    OP op;
    std::string operand;
    uint16_t operandAffi;
    uint8_t scope;
};

struct CodeGenerator
{
    SymbolTable* localSymtab;
    SymbolTable* symtab;
    NodeAllocator* allocator;
    InstructionSeq code;
    CpuState* cpu;
    OpDesc opDesc;
};

void generate_code(CodeGenerator* gen, std::vector<AstNode*>* parseTrees);
void dispatch(CodeGenerator* gen, AstNode* parseTree);
void write_to_file(const InstructionSeq& instructions, FILE* stream);
void write_label(const Instruction* inst, FILE* stream);
void write_instruction(const Instruction* inst, FILE* stream);
#endif