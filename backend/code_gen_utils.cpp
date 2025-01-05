#include "code_gen_utils.hpp"

using namespace std;

void fillTypeHwdInfo(SymbolType *symType)
{
}

uint32_t getTypeAlignment(SymbolType *symType)
{
    return 0;
}

std::string encodeAsAsmData(AstNode* dataNode)
{
    return std::string();
}

void zeroInitVariable(Instruction* inst, SymbolType* symType, const std::string symName)
{
    // before label
    inst->src += ".globl ";
    inst->src += symName;
    inst->src += '\0';
    inst->src += ".bss";
    inst->src += '\0';
    inst->src += ".align ";
    inst->src += to_string(symType->typeAlignment);
    inst->src += '\0';
    inst->src += ".type ";
    inst->src += symName;
    inst->src += ", @object";
    inst->src += '\0';
    inst->src += ".size ";
    inst->src += symName  + ", ";
    inst->src += to_string(symType->typeSize);
    inst->src += '\0';
    // after label
    inst->dest += ".zero ";
    inst->dest += to_string(symType->typeSize);
    inst->dest += '\0';
}
