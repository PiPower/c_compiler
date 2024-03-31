#ifndef CODE_GEN
#define CODE_GEN
#include "./code_generator_utils.hpp"
#include "../frontend/parser.hpp"


InstructionBuffer generateCode(std::vector<AstNode*>& instructionSequence);
int translate(InstructionBuffer& buffer, AstNode* root);
#endif