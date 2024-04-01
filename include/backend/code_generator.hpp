#ifndef CODE_GEN
#define CODE_GEN
#include "./code_generator_utils.hpp"
#include "../frontend/parser.hpp"

struct SymbolEntry
{
    // means entry is newly created, used to check if entry existed before called,
    // must be changed after creation
    int x = -1;
};
typedef std::unordered_map<std::string, SymbolEntry> SymbolTable;

InstructionBuffer generateCode(std::vector<AstNode*>& instructionSequence);
int translate(InstructionBuffer& buffer, AstNode* root);
#endif