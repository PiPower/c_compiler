#ifndef CODE_GEN
#define CODE_GEN
#include "./code_generator_utils.hpp"
#include "../frontend/parser.hpp"


char* generateCode(std::vector<AstNode*>& instructionSequence);

#endif