#include "code_gen_utils.hpp"
#include "code_gen.hpp"
#include "cpu.hpp"
using namespace std;

uint32_t getTypeAlignment(SymbolType *symType)
{
    uint32_t alignment = 1;
    while (alignment * 2 <= symType->typeSize && alignment * 2 < 64)
    {
        alignment*=2;
    }
    
    return alignment;
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
    inst->src += to_string(getTypeAlignment(symType));
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

uint8_t getAffiliationIndex(uint16_t typeGroup)
{
    uint8_t id = 0;
    while (typeGroup > 0)
    {
        typeGroup >>=1;
        id++;
    }
    return id;
}

uint8_t getTypeGroup(uint16_t affiliation)
{
    if( (affiliation & 0x0F) > 0)
    {
        return SIGNED_INT_GROUP;
    }
    if( (affiliation & (0x0F << 4)) > 0)
    {
        return UNSIGNED_INT_GROUP;
    }
    if( (affiliation &  (0x0F << 8)) > 0)
    {
        return FLOAT_GROUP;
    }
        
    return SPECIAL_GROUP;
}