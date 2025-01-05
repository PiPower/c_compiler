#include "code_gen_utils.hpp"
#include "code_gen.hpp"

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

Instruction generateFunctionLabel(AstNode *fnDef)
{
    Instruction fn;
    fn.type = LABEL;
    fn.mnemonic =  std::move(*fnDef->data);
    // before label
    fn.src += ".text";
    fn.src += '\0';
    fn.src += ".globl ";
    fn.src += fn.mnemonic;
    fn.src += '\0';
    fn.src += ".type ";
    fn.src += fn.mnemonic;
    fn.src += ", @function";
    fn.src += '\0';
    return fn;
}

void zeroInitVariable(Instruction* inst, SymbolType* symType, const std::string symName)
{
    // before label
    inst->src += ".bss";
    inst->src += '\0';
    inst->src += ".globl ";
    inst->src += symName;
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
