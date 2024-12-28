#ifndef CODE_GEN
#define CODE_GEN
#include "../frontend/parser.hpp"

struct Instruction
{
    std::string mnemonic;
    std::string source;
    std::string destination;
};

struct CodeGenerator
{
    SymbolTable* symtab;
    std::vector<AstNode*>* parseTrees;
    std::vector<Instruction> code;
};

char* generate_code(CodeGenerator* gen);
void dispatcher(CodeGenerator* gen, AstNode* parseTree);

#endif