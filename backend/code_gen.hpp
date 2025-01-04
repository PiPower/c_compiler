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


struct Instruction
{
    std::string mnemonic;
    std::string src;
    std::string dest;
};

struct CodeGenerator
{
    SymbolTable* scopedSymtab;
    SymbolTable* symtab;
    NodeAllocator* allocator;
    std::vector<AstNode*>* parseTrees;
    std::vector<Instruction> code;
    uint64_t scope;
};

char* generate_code(CodeGenerator* gen);
void dispatcher(CodeGenerator* gen, AstNode* parseTree);

#endif