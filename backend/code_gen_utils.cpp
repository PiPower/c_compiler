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

std::string generateOperand(const CpuState* cpu,const OpDesc& destDesc, int regByteSize)
{
    string dest;
    
    VariableCpuDesc desc = fetchVariable(cpu, destDesc.operand);
    switch( desc.storageType)
    {
        case Storage::REG:
        {
            if(regByteSize == -1)
            {
                regByteSize = desc.offset < 16 ? 0 : 2;
            }
            if(regByteSize == IDX_R8HI && desc.offset >= RSI)
            {
                printf("High 8 bits are not supported for register [%s]\n", cpu_registers_str[desc.offset][0]);
                exit(-1);
            }
            string out;
            out += '%';
            out += cpu_registers_str[desc.offset][regByteSize];
            return out;
        }break;
        case Storage::STACK:
        {
            dest = to_string(desc.offset) +
                        '('  + '%'+ cpu_registers_str[RBP][0] + ')';
            return dest;
        }break;
        case Storage::MEMORY:
            return destDesc.operand + "(%rip)";
            break;
    default:
        printf("Internal Error: storage type error \n");
        exit(-1);
        break;
    }
    return {};
}

std::string generateTmpVarname()
{
    static uint64_t id;
    std::string out = "tmp_var_" + to_string(id);
    id++;
    return out;
}

std::string generateRegisterName()
{
    static uint64_t id;
    std::string out = "_reg_" + to_string(id);
    id++;
    return out;
}

void convertToProperArithemticType(CodeGenerator *gen, OpDesc *srcDesc, uint16_t expectedAffi)
{
    OpDesc tmp;
    if(expectedAffi == INT64_S || expectedAffi == INT64_U)
    {   
        if(srcDesc->operandAffi == expectedAffi)
        {
            return;
        }
        tmp = generateTmpVar(expectedAffi, gen->localSymtab->scopeLevel);
        allocateRRegister(gen, tmp.operand);
        if(expectedAffi == INT64_S)
        {
            writeToSignedIntReg(gen, *srcDesc, tmp);
        }
        else
        {
            writeToUnsignedIntReg(gen, *srcDesc, tmp);
        }
        freeRegister(gen, srcDesc->operand);
        *srcDesc = tmp;
        return;
    }
    else if(expectedAffi == FLOAT32 || expectedAffi == DOUBLE64)
    {   
        if(srcDesc->operandAffi == expectedAffi)
        {
            return;
        }
        tmp = generateTmpVar(expectedAffi, gen->localSymtab->scopeLevel);
        allocateMMRegister(gen, tmp.operand);
        writeToFloatReg(gen, *srcDesc, tmp);
        freeRegister(gen, srcDesc->operand);
        *srcDesc = tmp;
        return;
    }
    else
    {
        printf("Error: Specified affilitation is NOT proper arithmetic type\n");
        exit(-1);
    }
}

void performArithmeticOp(CodeGenerator* gen, OpDesc* left, OpDesc* right, uint16_t affiliation,
                                std::string op_si, std::string op_ui, std::string op_f32, std::string op_d64)
{
    uint8_t opGr = getTypeGroup(affiliation);
    if(opGr == SIGNED_INT_GROUP)
    {
        convertToProperArithemticType(gen, left, INT64_S);
        convertToProperArithemticType(gen, right, INT64_S);
        ADD_INST(gen, {INSTRUCTION, std::move(op_si), generateOperand(gen->cpu, *left), generateOperand(gen->cpu, *right)});
    }
    else if(opGr == UNSIGNED_INT_GROUP)
    {
        convertToProperArithemticType(gen, left, INT64_U);
        convertToProperArithemticType(gen, right, INT64_U);
        ADD_INST(gen, {INSTRUCTION, std::move(op_ui), generateOperand(gen->cpu, *left), generateOperand(gen->cpu, *right)});
    }
    else if(affiliation == FLOAT32)
    {
        convertToProperArithemticType(gen, left, FLOAT32);
        convertToProperArithemticType(gen, right, FLOAT32);
        ADD_INST(gen, {INSTRUCTION,  std::move(op_f32), generateOperand(gen->cpu, *left), generateOperand(gen->cpu, *right)});
    }
    else if (affiliation == DOUBLE64)
    {
        convertToProperArithemticType(gen, left, DOUBLE64);
        convertToProperArithemticType(gen, right, DOUBLE64);
        ADD_INST(gen, {INSTRUCTION,  std::move(op_d64), generateOperand(gen->cpu, *left), generateOperand(gen->cpu, *right)});
    }
    else
    {
        printf("Error: unsupproted affiliation\n");
        exit(-1);
    }
}

OpDesc generateTmpVar(uint16_t affiliation, uint8_t scopeLvl)
{
    OpDesc destDesc = {
        .operandType = OP::TEMP_VAR,
        .operand = generateTmpVarname(),
        .operandAffi = affiliation,
        .scope = scopeLvl
    };
    return destDesc;
}

OpDesc parseEncodedAccess(CodeGenerator *gen, const std::string &accesSpec)
{
    if(accesSpec.find(':') == string::npos)
    {
        uint64_t scope = 0;
        SymbolVariable* symVar = (SymbolVariable*)GET_SCOPED_SYM_EX(gen, accesSpec, &scope);
        SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *symVar->varType);
        OpDesc desc;
        desc.operandType = OP::VARIABLE;
        desc.operand = accesSpec;
        desc.scope = scope;
        desc.operandAffi = symType->affiliation;
        return desc;
    }
    printf("Unsupported\n");
    exit(-1);
    return OpDesc();
}
