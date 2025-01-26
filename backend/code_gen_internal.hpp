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
OpDesc writeToLocalVariable(CodeGenerator* gen, const std::string& varname, OpDesc operandDesc);
OpDesc prepareConstant(CodeGenerator* gen, AstNode* parseTree);
OpDesc prepareVariable(CodeGenerator* gen, AstNode* parseTree);
OpDesc writeConstantToSym(CodeGenerator* gen, std::string constant, const std::string& dest);
OpDesc processChild(CodeGenerator* gen, AstNode* parseTree, std::size_t child_index);
void loadSignedInt(CodeGenerator *gen, const OpDesc &varDesc);
void loadUnsignedInt(CodeGenerator *gen, const OpDesc &varDesc);
void loadFloat(CodeGenerator *gen, const OpDesc &varDesc, uint16_t operationAffi);
//expressions
OpDesc translateGlobalInit(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateLocalInit(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateExpr(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateNegation(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateAssignment(CodeGenerator* gen, AstNode* parseTree);
OpDesc translateCast(CodeGenerator* gen, AstNode* parseTree);
// moves constant int to either register or memory location
// caller allocates either memory or register for destination
void moveConstantInt(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc);
// moves constant float to either register or memory location
// caller allocates either memory or register for destination
void moveConstantFloat(CodeGenerator* gen, const std::string& constant, const OpDesc &destDesc);
// loads integer into register
void loadVariableToReg(CodeGenerator* gen, const OpDesc &varDesc, uint16_t operationAffi);
uint8_t allocateRRegister(CodeGenerator* gen, std::string symName);
void freeRRegister(CodeGenerator* gen, int index);
void freeRRegister(CodeGenerator* gen, const std::string& symName);
uint8_t allocateMMRegister(CodeGenerator* gen, std::string symName);
void freeMMRegister(CodeGenerator* gen, int index);
void freeMMRegister(CodeGenerator* gen, const std::string& symName);
#endif