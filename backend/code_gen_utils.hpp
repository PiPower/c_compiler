#ifndef CODE_GEN_UTILS
#define CODE_GEN_UTILS
#include "../symbol_table.hpp"
#include "../frontend/parser.hpp"
#include "code_gen.hpp"

// alignment is first power of 2 that is <= type_size
uint32_t getTypeAlignment(SymbolType* symType);
Instruction generateFunctionLabel(AstNode* fnDef);
void zeroInitVariable(Instruction* inst, SymbolType* symType, const std::string symName);
uint8_t getAffiliationIndex(uint16_t typeGroup);
uint8_t getTypeGr(uint16_t affiliation);
int64_t encodeIntAsBinary(const std::string& constant);
uint64_t encodeFloatAsBinary(const std::string& constant, uint8_t floatSize);
std::string encodeIntAsString(long int constant, uint8_t byteSize);
std::string generateOperand(const CpuState* cpu, const OpDesc& destDesc, int regByteSize = -1);
std::string generateTmpVarname();
std::string generateRegisterName();
void convertToProperArithemticType(CodeGenerator* gen, OpDesc* srcDesc, uint16_t expectedAffi);
void performArithmeticOp(CodeGenerator* gen, OpDesc* left, OpDesc* right, uint16_t affiliation,
                            std::string op_si, std::string op_ui, std::string op_f32, std::string op_d64);
OpDesc generateTmpVar(uint16_t affiliation, uint8_t scopeLvl);
/*
to make it possible to acces struct the format is
name:name:...:name:variable
if variable is not part of struct only variable part can be specified
each name corresponds to one of nested structs
*/
OpDesc parseEncodedAccess(CodeGenerator* gen, const std::string& accesSpec);
#endif