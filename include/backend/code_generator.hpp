#ifndef CODE_GEN
#define CODE_GEN
#include "./code_generator_utils.hpp"
#include "../frontend/parser.hpp"
#define UNDEFINED_ENTRY (-1)

struct SymbolEntry
{
    // means entry is newly created, used to check if entry existed before called,
    // must be changed after creation
    int x = UNDEFINED_ENTRY;
};
typedef std::unordered_map<std::string, SymbolEntry> SymbolTable;

InstructionBuffer generateCode(std::vector<AstNode*>& instructionSequence);
int translate(InstructionBuffer& buffer, AstNode* root);
#endif