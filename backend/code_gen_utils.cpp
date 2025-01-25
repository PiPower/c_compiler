#include "code_gen_utils.hpp"
#include "code_gen.hpp"
#include "cpu.hpp"
#include "code_gen_internal.hpp"
#include <string.h>
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

uint8_t getTypeGr(uint16_t affiliation)
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

int64_t encodeIntAsBinary(const std::string &constant)
{
    const char* data = constant.c_str();
    int64_t sign = 1;
    int64_t value = 0;
    if(*data == '-')
    {
        sign = -1;
        data++;
    }
    while (*data >= '0' && *data <= '9')
    {
        value *= 10;
        value += *data - '0';
        data++;
    }
    value *= sign;
    return value;
}

uint64_t encodeFloatAsBinary(const std::string &constant, uint8_t floatSize)
{
    uint64_t val = 0;
    switch (floatSize)
    {
    case 4:
    {
        float buff = stof(constant);
        memcpy(&val, &buff, sizeof(float));
    }    break;
    case 8:
    {
        double buff = stod(constant);
        memcpy(&val, &buff, sizeof(double));
    }    break;
    default:    
        printf("Internal Error: Unsupported float type\n");
        exit(-1);
        break;
    }
    return val;
}

std::string encodeIntAsString(long int constant, uint8_t byteSize)
{
    if (byteSize > 8)
    {
        printf("Internal Error: byteSize cannot be larger than 8\n");
        exit(-1);
    }
    
    switch (byteSize)
    {
    case 1:
        {
            char x = (char)constant;
            memset(&constant, 0, sizeof(long int));
            constant = x;
        }
        break;
    case 2:
        {
            short x = (short)constant;
            memset(&constant, 0, sizeof(long int));
            constant = x;
        }
        break;
    case 4:
        {
            int x = (int)constant;
            memset(&constant, 0, sizeof(long int));
            constant = x;   
        }
        break;
    case 8:
        break;
    default:
        printf("Internal Error: Incorrect byteSize\n");
        exit(-1);
        break;
    }
    string constProcessed;
    constProcessed += '$';
    constProcessed += to_string(constant);
    
        
    return constProcessed;
}

std::string genAssignmentDest(const CpuState* cpu,const OpDesc& destDesc)
{
    string dest;

    if(destDesc.scope > 0)
    {
        auto varDescIter = cpu->data.find(destDesc.operand);
        const VariableDesc* desc = &varDescIter->second;
        switch( desc->storageType)
        {
        case Storage::REG:
        {
            string out;
            out += '%';
            out += cpu_registers_str[desc->offset][2];
            return out;
        }break;
        case Storage::STACK:
        {
            dest = to_string(desc->offset) +
                        '('  + '%'+ cpu_registers_str[RBP][0] + ')';
            return dest;
        }break;
        default:
            printf("Internal Error: storage type error \n");
            exit(-1);
            break;
        }
    }
    else
    {
        printf("Assignment to global not supported \n");
        exit(-1);
    }
}

OpDesc parseEncodedAccess(CodeGenerator *gen, const std::string &accesSpec)
{
    if(accesSpec.find(':') == string::npos)
    {
        uint64_t scope = 0;
        SymbolVariable* symVar = (SymbolVariable*)GET_SCOPED_SYM_EX(gen, accesSpec, &scope);
        SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *symVar->varType);
        OpDesc desc;
        desc.op = OP::VARIABLE;
        desc.operand = accesSpec;
        desc.scope = scope;
        desc.operandAffi = symType->affiliation;
        return desc;
    }
    printf("Unsupported\n");
    exit(-1);
    return OpDesc();
}
