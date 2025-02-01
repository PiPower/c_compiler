#ifndef CODE_GEN_INTERNAL
#define CODE_GEN_INTERNAL
#include "code_gen.hpp"
#include "../frontend/node_allocator.hpp"
#include "cpu.hpp"


#define GET_SCOPED_SYM(gen, name)  getSymbol((gen)->localSymtab, (name)) 
#define GET_SCOPED_SYM_EX(gen, name, scope)  getSymbol((gen)->localSymtab, name, scope) 
#define ADD_INST(gen, ...) (gen)->code.push_back(__VA_ARGS__ )
#define ADD_INST_MV(gen, inst) (gen)->code.push_back(std::move(inst))
#define FREE_NODE(gen, node) freeNode((gen)->allocator, (node))
#define FREE_NODE_REC(gen, node) freeRecursive((gen)->allocator, (node))
// symbol table variable translation
OpDesc translateFunction(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateDeclaration(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateExpression(CodeGenerator* gen, AstNode* parseTree);
OpDesc prepareConstant(CodeGenerator* gen, AstNode* parseTree);
OpDesc prepareVariable(CodeGenerator* gen, AstNode* parseTree);
OpDesc processChild(CodeGenerator* gen, AstNode* parseTree, std::size_t child_index, 
                                                bool loadConst = true, bool loadVarToReg = true);
//expressions
OpDesc translateGlobalInit(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateLocalInit(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateExpr(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateNegation(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateAssignment(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateCast(CodeGenerator* gen, AstNode* parseTree);
uint8_t allocateRRegister(CodeGenerator* gen, std::string symName);
uint8_t allocateMMRegister(CodeGenerator* gen, std::string symName);
void freeRegister(CodeGenerator* gen, int index);
void freeRegister(CodeGenerator* gen, const std::string& symName);

// load API
// writes constant into memory/register
OpDesc loadConstant(CodeGenerator* gen, std::string constant, const OpDesc &destDesc);
void loadConstantInt(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc);
void loadConstantFloat(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc);
// writes register into memory reg -> mem
OpDesc writeRegToMem(CodeGenerator* gen, const OpDesc& srcDesc, const OpDesc &destDesc);
void writeSignedIntToMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc);
void writeUnsignedIntToMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc);
void writeFloatToMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc);
// write memory/register into reg
OpDesc loadVariableToReg(CodeGenerator* gen, const OpDesc &varDesc, uint16_t operationAffi);
void loadSignedIntToReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc);
void loadUnsignedIntToReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc);
void loadFloatToReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc);
#endif