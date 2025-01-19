#ifndef CODE_GEN_UTILS
#define CODE_GEN_UTILS
#include "../symbol_table.hpp"
#include "../frontend/parser.hpp"
#include "code_gen.hpp"

#define CLEAR_OP(gen) (gen)->opDesc.op = OP::NONE;  (gen)->opDesc.operand.clear();
// alignment is first power of 2 that is <= type_size
uint32_t getTypeAlignment(SymbolType* symType);
Instruction generateFunctionLabel(AstNode* fnDef);
void zeroInitVariable(Instruction* inst, SymbolType* symType, const std::string symName);
uint8_t getAffiliationIndex(uint16_t typeGroup);
uint8_t getTypeGr(uint16_t affiliation);
long unsigned int encodeAsUnsignedBin(const std::string& constant);
long int encodeAsSignedBin(const std::string& constant);
std::string encodeIntAsString(long int constant, uint8_t byteSize);

/*
to make it possible to acces struct the format is
name:name:...:name:variable
if variable is not part of struct only variable can be specified
each name corresponds to one of nested structs

on return operand holds bytes for either address or 
*/
OpDesc parseEncodedAccess(CodeGenerator* gen, const std::string& accesSpec);
#endif