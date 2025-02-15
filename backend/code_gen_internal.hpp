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
#define FREE_DISPATCH(gen, parseTree) freeRegister(gen, dispatch(gen, parseTree).operand )

// register manipulation
OpDesc assertThatRegIsOfAffi(uint8_t affiliaton, CpuState* cpu, OpDesc varDesc);
uint8_t allocateRRegister(CodeGenerator* gen, std::string symName);
uint8_t allocateMMRegister(CodeGenerator* gen, std::string symName);
void freeRegister(CodeGenerator* gen, int index);
void freeRegister(CodeGenerator* gen, const std::string& symName);

// symbol table variable translation
OpDesc translateFunction(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateDeclaration(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateExpression(CodeGenerator* gen, AstNode* parseTree);
OpDesc prepareConstant(CodeGenerator* gen, AstNode* parseTree);
OpDesc prepareVariable(CodeGenerator* gen, AstNode* parseTree);
OpDesc processChild(CodeGenerator* gen, AstNode* parseTree, std::size_t child_index, 
                                                bool loadConst = true, bool loadVarToReg = true);
//statements
OpDesc translateIfStmt(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateConditionalJmp(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateBlock(CodeGenerator* gen, AstNode* block);
OpDesc translateForLoop(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateWhileLoop(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateDoWhileLoop(CodeGenerator* gen, AstNode* parseTree);
//expressions
OpDesc translateComparison(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateGlobalInit(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateLocalInit(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateExpr(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateNegation(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateAssignment(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateCast(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateMultiplication(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateAddition(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateSubtraction(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateBitwiseOp(CodeGenerator* gen, AstNode* parseTree);
// load API
// writes constant into memory/register
OpDesc writeConstant(CodeGenerator* gen, std::string constant, const OpDesc &destDesc);
void writeConstantInt(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc);
void writeConstantFloat(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc);
// writes  register into mem
// each register variable is assumed to be either 64bit sint/uint or float32 or float64
OpDesc writeRegToMem(CodeGenerator* gen, const OpDesc& srcDesc, const OpDesc &destDesc);
void writeToSignedIntMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc);
void writeToUnsignedIntMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc);
void writeToFloatMem(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc &destDesc);
// write memory/register into reg
OpDesc writeVariableToReg(CodeGenerator* gen, const OpDesc &varDesc, const OpDesc& destDesc);
void writeToSignedIntReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc);
void writeToUnsignedIntReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc);
void writeToFloatReg(CodeGenerator *gen, const OpDesc& srcDesc, const OpDesc& destDesc);
#endif