#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
using namespace std;

OpDesc translateExpr(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::MINUS:
        return translateNegation(gen, parseTree);
    case NodeType::ASSIGNMENT:
        return translateAssignment(gen, parseTree);
    case NodeType::CAST:
        return translateCast(gen, parseTree);
    case NodeType::MULTIPLY:
        return translateMultiplication(gen, parseTree);
    case NodeType::ADD:
        return translateAddition(gen, parseTree);
    case NodeType::SUBTRACT:
        return translateSubtraction(gen, parseTree);
    case NodeType::AND:
    case NodeType::OR:
    case NodeType::EXC_OR:
        return translateBitwiseOp(gen, parseTree);
    case NodeType::GREATER:
    case NodeType::GREATER_EQUAL:
    case NodeType::EQUAL:
    case NodeType::NOT_EQUAL:
    case NodeType::LESS:
    case NodeType::LESS_EQUAL:
        return translateComparison(gen, parseTree);
    default:
        printf("Unused node type \n");
        break;
        //exit(-1);
    }
    return {OP::NONE};
}

OpDesc translateNegation(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = processChild(gen, parseTree, 0);
    if(opDesc.operandType == OP::CONSTANT)
    {
       opDesc.operand.insert(0, 1, '-');
       return opDesc;
    }

    //VariableCpuDesc cpuDesc = fetchVariable(gen->cpu, opDesc.operand);
    return opDesc;
}

OpDesc translateAssignment(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc rightSide = processChild(gen, parseTree, 1);
    OpDesc destDesc = processChild(gen, parseTree, 0, true, false);
    writeRegToMem(gen, rightSide, destDesc);
    freeRegister(gen, rightSide.operand);
    return destDesc;
}

OpDesc translateCast(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = processChild(gen, parseTree, 0);
    //uint8_t gr = getTypeGroup()
    return opDesc;
}

OpDesc translateMultiplication(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc left = processChild(gen, parseTree, 0);
    OpDesc right = processChild(gen, parseTree, 1);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
    performArithmeticOp(gen, &left, &right, symType->affiliation, "imulq", "imulq", "mulss", "mulsd");
    freeRegister(gen, left.operand);
    return right;
}

OpDesc translateAddition(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc left = processChild(gen, parseTree, 0);
    OpDesc right = processChild(gen, parseTree, 1);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
    performArithmeticOp(gen, &left, &right, symType->affiliation, "addq", "addq", "addss", "addsd");
    freeRegister(gen, left.operand);
    return right;
}

OpDesc translateSubtraction(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc left = processChild(gen, parseTree, 0);
    OpDesc right = processChild(gen, parseTree, 1);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
    performArithmeticOp(gen, &left, &right, symType->affiliation, "subq", "subq", "subss", "subsd");
    freeRegister(gen, left.operand);
    return right;
}

OpDesc translateBitwiseOp(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc left = processChild(gen, parseTree, 0);
    OpDesc right = processChild(gen, parseTree, 1);
    uint8_t lGr = getTypeGroup(left.operandAffi), rGr = getTypeGroup(right.operandAffi);
    if( (lGr != SIGNED_INT_GROUP && lGr != UNSIGNED_INT_GROUP)
        ||
        (rGr != SIGNED_INT_GROUP && rGr != UNSIGNED_INT_GROUP) )
    {
        printf("Internal Error: Forbidden type in bitwise and op\n");
        exit(-1);
    }

    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
    if(parseTree->nodeType == NodeType::AND)
    {
        performArithmeticOp(gen, &left, &right, symType->affiliation, "andq", "andq", "", "");
    }
    else if(parseTree->nodeType == NodeType::OR)
    {
        performArithmeticOp(gen, &left, &right, symType->affiliation, "orq ", "orq ", "", "");
    }
    else 
    {
        performArithmeticOp(gen, &left, &right, symType->affiliation, "xorq", "xorq", "", "");
    }
    freeRegister(gen, left.operand);
    return right;
}

OpDesc translateComparison(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc left = processChild(gen, parseTree, 0);
    OpDesc right = processChild(gen, parseTree, 1);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);

    if(symType->affiliation == FLOAT32 || 
       symType->affiliation == DOUBLE64)
    {
        OpDesc buff = generateSetFloat(parseTree->nodeType, gen, &left, &right, symType->affiliation);
        freeRegister(gen, right.operand);
        right = buff;
    }
    else if(symType->affiliation == INT64_U ||
            symType->affiliation == INT64_S)
    {
        generateSetInt(parseTree->nodeType, gen, &left, &right, symType->affiliation);
    }
    else
    {
        printf("Unsupported type \n");
        exit(-1);
    }
   
    freeRegister(gen, left.operand);
    return right;
}

OpDesc writeConstant(CodeGenerator *gen, std::string constant, const OpDesc &destDesc)
{
    uint8_t gr = getTypeGr(destDesc.operandAffi);

    if(gr == SIGNED_INT_GROUP || gr == UNSIGNED_INT_GROUP)
    {
        writeConstantInt(gen, constant, destDesc);
    }
    else if (gr == FLOAT_GROUP)
    {
        writeConstantFloat(gen, constant, destDesc);
    }
    else
    {
        printf("Internal Error: Unsupported group\n");
        exit(-1);
    }
    return destDesc;
}


void writeConstantInt(CodeGenerator *gen, const std::string &constant, const OpDesc &destDesc)
{
    //first process value to be sure it is in correct range
    long int value = encodeIntAsBinary(constant);
    Instruction inst;
    inst.type = INSTRUCTION;
    int regNr = -1;
    int regIdx = 0;
    if(destDesc.operandAffi == INT8_S || destDesc.operandAffi == INT8_U)
    {
        inst.mnemonic = "movb";
        inst.src = encodeIntAsString(value, 1);
    }
    else if(destDesc.operandAffi  == INT16_S || destDesc.operandAffi == INT16_U)
    {
        inst.mnemonic = "movw";
        inst.src = encodeIntAsString(value, 2);
    }
    else if(destDesc.operandAffi  == INT32_S  || destDesc.operandAffi == INT32_U)
    {
        inst.mnemonic = "movl";
        regIdx = IDX_R32;
        inst.src = encodeIntAsString(value, 4);
    }
    else if(destDesc.operandAffi  == INT64_S ||  destDesc.operandAffi == INT64_U)
    {
        VariableCpuDesc varCpu = fetchVariable(gen->cpu, destDesc.operand);
        regIdx = IDX_R64;
        if( (uint64_t)(value >> 32 ) > 0)
        {
            if(varCpu.storageType != Storage::REG)
            {
                regNr = (int)allocateRRegister(gen, "init_tmp");
                Instruction instLocal = {INSTRUCTION, "movabsq", 
                                    encodeIntAsString(value, 8), 
                                    cpu_registers_str[regNr][0] };
                instLocal.dest = '%' + instLocal.dest;
                inst.src = instLocal.dest;
                inst.mnemonic = "movq";
                ADD_INST_MV(gen, instLocal);
            }
            else
            {
                inst.mnemonic = "movabsq";
            }
        }
        else
        {   
            inst.mnemonic = "movq";
            inst.src = encodeIntAsString(value, 8);
        }
    }

    VariableCpuDesc cpuDesc = fetchVariable(gen->cpu, destDesc.operand);
    if(cpuDesc.storageType == Storage::REG
        && (destDesc.operandAffi  == INT16_S || destDesc.operandAffi == INT16_U ||
            destDesc.operandAffi == INT8_S || destDesc.operandAffi == INT8_U) )
    {
        inst.mnemonic = "movl";
        regIdx =  IDX_R32;
    }

    inst.dest = generateOperand(gen->cpu, destDesc, regIdx);
    ADD_INST_MV(gen, inst);
    freeRegister(gen, regNr);
}

void writeConstantFloat(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc)
{
    int reg = -1;
    OpDesc tempDesc = { .operandType = OP::NONE};
    VariableCpuDesc varCpu = fetchVariable(gen->cpu, destDesc.operand);
    if(varCpu.storageType != Storage::REG)
    {
        tempDesc = {
            .operandType = OP::TEMP_VAR,
            .operand =  "init_float_tmp",
            .operandAffi = destDesc.operandAffi,
            .scope = destDesc.scope
        };
        reg = (int)allocateMMRegister(gen, "init_float_tmp");
    }

    string label;
    uint64_t val = 0;
    Instruction movToReg = {
        .type = INSTRUCTION
    }; 

    if(destDesc.operandAffi == FLOAT32)
    {
        val = encodeFloatAsBinary(constant, 4);
        movToReg.mnemonic = "movss";
    }
    else if(destDesc.operandAffi == DOUBLE64 )
    {
        val = encodeFloatAsBinary(constant, 8);
        movToReg.mnemonic = "movsd";
    }
    else
    {
        printf("Internal error: Unsupported float type \n");
        exit(-1);
    }

    auto floatConst = gen->floatConsts.find(val);
    if(floatConst == gen->floatConsts.cend())
    {
        label = generateLocalConstantLabel();
        gen->floatConsts[val] = label;
    }
    else
    {
        label = gen->floatConsts[val];
    }

    movToReg.src = label + "(%rip)";
    if(tempDesc.operandType == OP::NONE)
    {
        movToReg.dest = generateOperand(gen->cpu, destDesc);
    }
    else
    {
        movToReg.dest = generateOperand(gen->cpu, tempDesc);
    }

    ADD_INST_MV(gen, movToReg);
    if(tempDesc.operandType != OP::NONE)
    {
        const Instruction& prev = gen->code.back();
        Instruction moveToTarget = {
            .type = INSTRUCTION,
            .mnemonic = prev.mnemonic,
            .src = prev.dest,
            .dest = generateOperand(gen->cpu, destDesc)
        };
        ADD_INST_MV(gen, moveToTarget);
    }
    freeRegister(gen, reg);
}

void writeToSignedIntReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc)
{
    string src = generateOperand(gen->cpu, srcDesc);
    Instruction loadInst;
    switch (srcDesc.operandAffi)
    {
    case INT8_S:
        ADD_INST(gen, {INSTRUCTION, "movsbq", generateOperand(gen->cpu, srcDesc, IDX_R8LO),
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT16_S:
        ADD_INST(gen,  {INSTRUCTION, "movswq", generateOperand(gen->cpu, srcDesc, IDX_R16),
                                                            generateOperand(gen->cpu, destDesc)});
        break;
    case INT32_S:
        if(registerStores(gen->cpu, RAX, destDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, srcDesc, IDX_R32), "%eax"});
            ADD_INST(gen, {INSTRUCTION, "cltq"});
        }
        else if(registerStores(gen->cpu, RAX, srcDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "movq", generateOperand(gen->cpu, srcDesc), 
                                                        generateOperand(gen->cpu, destDesc)}); // preserve original value in reg
            ADD_INST(gen, {INSTRUCTION, "cltq"});
            ADD_INST(gen, {INSTRUCTION, "xchg", generateOperand(gen->cpu, srcDesc),
                                                                   generateOperand(gen->cpu, destDesc) });
        }
        else
        {
            if(isRegisterUsed(gen->cpu, RAX))
            {
                ADD_INST(gen, {INSTRUCTION, "pushq", "%rax"});
            }
            ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, srcDesc, IDX_R32), "%eax"});
            ADD_INST(gen, {INSTRUCTION, "cltq"});
            ADD_INST(gen, {INSTRUCTION, "movq", "%rax", generateOperand(gen->cpu, destDesc)});
            if(isRegisterUsed(gen->cpu, RAX))
            {
                ADD_INST(gen, {INSTRUCTION, "popq", "%rax"});
            }
        }
        break;
    case INT64_S:
        ADD_INST(gen, {INSTRUCTION, "movq", generateOperand(gen->cpu, srcDesc),
                                                generateOperand(gen->cpu, destDesc)});
        break; 
    case INT8_U:
        ADD_INST(gen, {INSTRUCTION, "movzbq", generateOperand(gen->cpu, srcDesc, IDX_R8LO),
                                                            generateOperand(gen->cpu, destDesc)});
        break;
    case INT16_U:
        ADD_INST(gen, {INSTRUCTION, "movzwq", generateOperand(gen->cpu, srcDesc, IDX_R16),
                                                            generateOperand(gen->cpu, destDesc)});
        break;
    case INT32_U:
        ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, srcDesc, IDX_R32),
                                                 generateOperand(gen->cpu, destDesc, IDX_R32)});
        break;
    case INT64_U:
        ADD_INST(gen, {INSTRUCTION, "movq", generateOperand(gen->cpu, srcDesc),
                                                generateOperand(gen->cpu, destDesc)});
        break;
    case FLOAT32:
        {
            uint8_t mmReg = allocateMMRegister(gen, "float_to_sint_temp");
            ADD_INST(gen, {INSTRUCTION, "movss", std::move(src), '%' + getCpuRegStr(mmReg, IDX_XMM)});
            ADD_INST(gen, {INSTRUCTION, "cvttss2siq", '%' + getCpuRegStr(mmReg, IDX_XMM), 
                                                            generateOperand(gen->cpu, destDesc)});
            freeRegister(gen, mmReg);
        }
        break;
    case DOUBLE64:
        {
            uint8_t mmReg = allocateMMRegister(gen, "float_to_sint_temp");
            ADD_INST(gen, {INSTRUCTION, "movsd", std::move(src), '%' + getCpuRegStr(mmReg, IDX_XMM)});
            ADD_INST(gen, {INSTRUCTION, "cvttsd2siq", '%' + getCpuRegStr(mmReg, IDX_XMM), 
                                                            generateOperand(gen->cpu, destDesc)});
            freeRegister(gen, mmReg);
        }
        break;
    }
}

void writeToUnsignedIntReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc)
{
    string src = generateOperand(gen->cpu, srcDesc);
    Instruction loadInst;
    switch (srcDesc.operandAffi)
    {
    case INT8_S:
        ADD_INST(gen, {INSTRUCTION, "movsbq", generateOperand(gen->cpu, destDesc, IDX_R8LO),
                                                            generateOperand(gen->cpu, destDesc) });
        break;
    case INT16_S:
        ADD_INST(gen, {INSTRUCTION, "movswq", generateOperand(gen->cpu, destDesc, IDX_R16),
                                                            generateOperand(gen->cpu, destDesc) });
        break;
    case INT32_S:
        if(registerStores(gen->cpu, RAX, destDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, srcDesc, IDX_R32), "%eax"});
            ADD_INST(gen, {INSTRUCTION, "cltq"});
        }
        else if(registerStores(gen->cpu, RAX, srcDesc.operand))
        {
            ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, srcDesc, IDX_R32), 
                                                        generateOperand(gen->cpu, destDesc, IDX_R32)}); // preserve original value in reg
            ADD_INST(gen, {INSTRUCTION, "cltq"});
            ADD_INST(gen, {INSTRUCTION, "xchg", generateOperand(gen->cpu, srcDesc),
                                                                   generateOperand(gen->cpu, destDesc) });
        }
        else
        {
            if(isRegisterUsed(gen->cpu, RAX))
            {
                ADD_INST(gen, {INSTRUCTION, "pushq", "%rax"});
            }
            ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, srcDesc, IDX_R32), "%eax"});
            ADD_INST(gen, {INSTRUCTION, "cltq"});
            ADD_INST(gen, {INSTRUCTION, "movq", "%rax", generateOperand(gen->cpu, destDesc)});
            if(isRegisterUsed(gen->cpu, RAX))
            {
                ADD_INST(gen, {INSTRUCTION, "popq", "%rax"});
            }
        }
        break;
    case INT64_S:
        ADD_INST(gen, {INSTRUCTION, "movq", generateOperand(gen->cpu, srcDesc),
                                                    generateOperand(gen->cpu, destDesc)});
        break; 
    case INT8_U:
        ADD_INST(gen,  {INSTRUCTION, "movzbq", generateOperand(gen->cpu, srcDesc, IDX_R8LO),
                                                                    generateOperand(gen->cpu, destDesc)});
        break;
    case INT16_U:
        ADD_INST(gen, {INSTRUCTION, "movzwq", generateOperand(gen->cpu, srcDesc, IDX_R16),
                                                                generateOperand(gen->cpu, destDesc)});
        break;
    case INT32_U:
        ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, srcDesc, IDX_R32),
                                                generateOperand(gen->cpu, destDesc, IDX_R32)});
        break;
    case INT64_U:
        ADD_INST(gen, {INSTRUCTION, "movq", generateOperand(gen->cpu, srcDesc),
                                                 generateOperand(gen->cpu, destDesc)});
        break;
    case FLOAT32:
        printf("Error: float to unsigned conversion is not supported\n");
        exit(-1);
        break;
    case DOUBLE64:
        printf("Error: double to unsigned conversion is not supported\n");
        exit(-1);
        break;
    }
}

void writeToFloatReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc)
{
    string src = generateOperand(gen->cpu, srcDesc);
    OpDesc tmp = srcDesc;
    tmp.operand += generateTmpVarname();
    Instruction loadInst;
    const char* mnemonic = nullptr;
    int regR = -1;
    switch (srcDesc.operandAffi)
    {
    case INT8_S:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movsbl", src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT16_S:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movswl",src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT32_S:
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, src, generateOperand(gen->cpu, destDesc)});
        break;
    case INT64_S:
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssq" : "cvtsi2sdq";
        ADD_INST(gen, {INSTRUCTION, mnemonic, src, generateOperand(gen->cpu, destDesc)});
        break; 
    case INT8_U:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movzbl",src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT16_U:
        regR = allocateRRegister(gen, tmp.operand);
        ADD_INST(gen, {INSTRUCTION, "movzwl",src, generateOperand(gen->cpu, tmp, IDX_R32)});
        ADD_INST(gen, {INSTRUCTION, "pxor",generateOperand(gen->cpu, destDesc), generateOperand(gen->cpu, destDesc)});
        mnemonic = destDesc.operandAffi == FLOAT32 ? "cvtsi2ssl" : "cvtsi2sdl";
        ADD_INST(gen, {INSTRUCTION, mnemonic, generateOperand(gen->cpu, tmp, IDX_R32), 
                                                             generateOperand(gen->cpu, destDesc)});
        break;
    case INT32_U:
        printf("Error: unsigned to float conversion is not supported\n");
        exit(-1);
        break;
    case INT64_U:
        printf("Error: unsigned long to float conversion is not supported\n");
        exit(-1);
        break;
    case FLOAT32:
        if(destDesc.operandAffi == FLOAT32)
        {
            ADD_INST(gen, {INSTRUCTION, "movss", src, generateOperand(gen->cpu, destDesc)});
        }
        else
        {
            ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, destDesc), 
                                                    generateOperand(gen->cpu, destDesc)});
            ADD_INST(gen, {INSTRUCTION, "cvtss2sd", src, generateOperand(gen->cpu, destDesc)});
        }
        break;
    case DOUBLE64:
        if(destDesc.operandAffi == DOUBLE64)
        {
            ADD_INST(gen, {INSTRUCTION, "movsd", src, generateOperand(gen->cpu, destDesc)});
        }
        else
        {
            ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, destDesc), 
                                                    generateOperand(gen->cpu, destDesc)});
            ADD_INST(gen, {INSTRUCTION, "cvtsd2ss", src, generateOperand(gen->cpu, destDesc)});
        }
        break;
    }
    freeRegister(gen, regR);
}

OpDesc writeRegToMem(CodeGenerator *gen, const OpDesc &srcDesc, const OpDesc &destDesc)
{
    uint8_t gr = getTypeGroup(destDesc.operandAffi);
    if(fetchVariable(gen->cpu, destDesc.operand).storageType == Storage::REG)
    {
        printf("Internal error: Storage  type is not a memory\n");
        exit(-1);
    }
    switch (gr)
    {
    case SIGNED_INT_GROUP:
        writeToSignedIntMem(gen, srcDesc, destDesc);
        break;
    case UNSIGNED_INT_GROUP:
        writeToUnsignedIntMem(gen, srcDesc, destDesc);
        break;
    case FLOAT_GROUP:
        writeToFloatMem(gen, srcDesc, destDesc);
        break;
    default:
        printf("Internal error: Unsupported group in assignment\n");
        exit(-1);
        break;
    }
    return destDesc;
}

void writeToSignedIntMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc)
{
    OpDesc tmp = {OP::NONE};
    OpDesc usedDesc = srcDesc;

    if(getTypeGroup(srcDesc.operandAffi) == FLOAT_GROUP)
    {
        //TODO when 32 bit int is considered value is converted to 64 bit which may cause some weird errors
        tmp = generateTmpVar(INT64_S, gen->localSymtab->scopeLevel);
        allocateRRegister(gen, generateTmpVarname());
        writeToSignedIntReg(gen, srcDesc, tmp);
        usedDesc = tmp;
    }

    switch (destDesc.operandAffi)
    {
    case INT8_U:
    case INT8_S:
        ADD_INST(gen, {INSTRUCTION, "movb", generateOperand(gen->cpu, usedDesc, IDX_R8LO), 
                                                    generateOperand(gen->cpu, destDesc, IDX_R8LO)});
        break;
    case INT16_U:
    case INT16_S:
        ADD_INST(gen, {INSTRUCTION, "movw", generateOperand(gen->cpu, usedDesc, IDX_R16), 
                                                generateOperand(gen->cpu, destDesc, IDX_R16)});
        break;
    case INT32_U:
    case INT32_S:
        ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, usedDesc, IDX_R32), 
                                                generateOperand(gen->cpu, destDesc, IDX_R32)});
        break;
    case INT64_U:
    case INT64_S:
        ADD_INST(gen, {INSTRUCTION, "movq", generateOperand(gen->cpu, usedDesc), 
                                                generateOperand(gen->cpu, destDesc)});
        break; 
    }

    if(tmp.operandType != OP::NONE)
    {
        freeRegister(gen, tmp.operand);
    }
}

void writeToUnsignedIntMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc)
{
    OpDesc tmp = {OP::NONE};
    OpDesc usedDesc = srcDesc;
    if(getTypeGroup(srcDesc.operandAffi) == FLOAT_GROUP)
    {
        //TODO when 32 bit int is considered value is converted to 64 bit which may cause some weird errors
        tmp = generateTmpVar(INT64_U, gen->localSymtab->scopeLevel);
        allocateRRegister(gen, generateTmpVarname());
        writeToUnsignedIntReg(gen, srcDesc, destDesc);
        usedDesc = tmp;
    }
    
    switch (destDesc.operandAffi)
    {
    case INT8_U:
    case INT8_S:
        ADD_INST(gen, {INSTRUCTION, "movb", generateOperand(gen->cpu, usedDesc, IDX_R8LO), 
                                                    generateOperand(gen->cpu, destDesc, IDX_R8LO)});
        break;
    case INT16_U:
    case INT16_S:
        ADD_INST(gen, {INSTRUCTION, "movw", generateOperand(gen->cpu, usedDesc, IDX_R16), 
                                                generateOperand(gen->cpu, destDesc, IDX_R16)});
        break;
    case INT32_U:
    case INT32_S:
        ADD_INST(gen, {INSTRUCTION, "movl", generateOperand(gen->cpu, usedDesc, IDX_R32), 
                                                generateOperand(gen->cpu, destDesc, IDX_R32)});
        break;
    case INT64_U:
    case INT64_S:
        ADD_INST(gen, {INSTRUCTION, "movq", generateOperand(gen->cpu, usedDesc), 
                                                generateOperand(gen->cpu, destDesc)});
        break; 
    }

    if(tmp.operandType != OP::NONE)
    {
        freeRegister(gen, tmp.operand);
    }
}

void writeToFloatMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc)
{
    OpDesc tmp = {OP::NONE};
    OpDesc usedDesc = srcDesc;
    if(getTypeGroup(srcDesc.operandAffi) == SIGNED_INT_GROUP || 
        getTypeGroup(srcDesc.operandAffi) == UNSIGNED_INT_GROUP )
    {
        //TODO when 32 bit int is considered value is converted to 64 bit which may cause some weird errors
        tmp = generateTmpVar(destDesc.operandAffi, gen->localSymtab->scopeLevel);
        allocateMMRegister(gen, generateTmpVarname());
        writeToFloatReg(gen, srcDesc, destDesc);
        usedDesc = tmp;
    }

    switch (usedDesc.operandAffi)
    {
    case FLOAT32:
    {
        OpDesc tmpVar;
        if(usedDesc.operandAffi != FLOAT32)
        {
            tmpVar = generateTmpVar(DOUBLE64, gen->localSymtab->scopeLevel);
            allocateMMRegister(gen, tmpVar.operand);
            ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, tmpVar), 
                                                generateOperand(gen->cpu, tmpVar)});
            ADD_INST(gen, {INSTRUCTION, "cvtss2sd", generateOperand(gen->cpu, usedDesc), 
                                                generateOperand(gen->cpu, tmpVar)});
            ADD_INST(gen, {INSTRUCTION, "movsd", generateOperand(gen->cpu, tmpVar), 
                                                    generateOperand(gen->cpu, destDesc)});
            freeRegister(gen, tmpVar.operand);
        }
        else
        {
            ADD_INST(gen, {INSTRUCTION, "movss", generateOperand(gen->cpu, usedDesc), 
                                                    generateOperand(gen->cpu, destDesc)});
        }
    }    break;
    case DOUBLE64:
    {
        OpDesc tmpVar;
        if(usedDesc.operandAffi != DOUBLE64)
        {
            tmpVar = generateTmpVar(FLOAT32, gen->localSymtab->scopeLevel);
            allocateMMRegister(gen, tmpVar.operand);
            ADD_INST(gen, {INSTRUCTION, "pxor", generateOperand(gen->cpu, tmpVar), 
                                                generateOperand(gen->cpu, tmpVar)});
            ADD_INST(gen, {INSTRUCTION, "cvtsd2ss", generateOperand(gen->cpu, usedDesc), 
                                                generateOperand(gen->cpu, tmpVar)});
            ADD_INST(gen, {INSTRUCTION, "movss", generateOperand(gen->cpu, tmpVar), 
                                                    generateOperand(gen->cpu, destDesc)});
            freeRegister(gen, tmpVar.operand);
        }
        else
        {
            ADD_INST(gen, {INSTRUCTION, "movsd", generateOperand(gen->cpu, usedDesc), 
                                                    generateOperand(gen->cpu, destDesc)});
        }
    }break;
    }

    if(tmp.operandType != OP::NONE)
    {
        freeRegister(gen, tmp.operand);
    }
}

OpDesc writeVariableToReg(CodeGenerator *gen, const OpDesc &varDesc, const OpDesc& destDesc)
{
    if(!(destDesc.operandAffi == INT64_S || destDesc.operandAffi == INT64_U ||
        destDesc.operandAffi == FLOAT32 || destDesc.operandAffi == DOUBLE64))
    {
        printf("Register has forbidden affiliation\n");
        exit(-1);
    }

    uint8_t targetGr = getTypeGr(destDesc.operandAffi);
    if(targetGr == SIGNED_INT_GROUP)
    {
        writeToSignedIntReg(gen, varDesc, destDesc);
    }
    else if(targetGr == UNSIGNED_INT_GROUP)
    {
        writeToUnsignedIntReg(gen, varDesc, destDesc);
    }
    else if(targetGr == FLOAT_GROUP)
    {
        writeToFloatReg(gen, varDesc, destDesc);
    }
    else
    {
        printf("Unsupported group \n");
        exit(-1);
    }
    
    return destDesc;
}

OpDesc assertThatRegIsOfAffi(uint8_t affiliaton, CpuState *cpu, OpDesc varDesc)
{
    return OpDesc();
}

uint8_t allocateRRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i =0; i < R15; i++)
    {
        uint8_t regIdx = (i + R9)%R15;
        if(gen->cpu->reg[regIdx].state == REG_FREE)
        {
            gen->cpu->regData[symName] = {
                .storageType = Storage::REG,
                .offset = regIdx
            };
            gen->cpu->reg[regIdx].state = REG_USED;
            gen->cpu->reg[regIdx].symbol = std::move(symName);
            return regIdx;
        }
    }
    printf("No free Registers \n");
    exit(-1);
}

void freeRegister(CodeGenerator *gen, int index)
{
    if(index < 0)
    {
        return;
    }
    gen->cpu->regData.erase(gen->cpu->reg[index].symbol);
    gen->cpu->reg[index].state = REG_FREE;
}

void freeRegister(CodeGenerator *gen, const std::string &symName)
{
    for(uint8_t i = RAX; i < XMM15; i++)
    {
        if(gen->cpu->reg[i].symbol == symName)
        {
            gen->cpu->regData.erase(gen->cpu->reg[i].symbol);
            gen->cpu->reg[i].state = REG_FREE;
            return;
        }
    }
}

uint8_t allocateMMRegister(CodeGenerator *gen, std::string symName)
{
    for(uint8_t i = XMM0; i <= XMM15; i++)
    {
        if(gen->cpu->reg[i].state == REG_FREE)
        {
            gen->cpu->regData[symName] = {
                .storageType = Storage::REG,
                .offset = i
            };
            gen->cpu->reg[i].state = REG_USED;
            gen->cpu->reg[i].symbol = std::move(symName);
            return i;
        }
    }
    printf("No free MM Registers \n");
    exit(-1);
}
