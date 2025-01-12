#ifndef CODE_GEN_INTERNAL
#define CODE_GEN_INTERNAL
#include "code_gen.hpp"
#include "../frontend/node_allocator.hpp"
#include "cpu.hpp"


#define GET_SCOPED_SYM(gen, name)  getSymbol((gen)->localSymtab, (name)) 
#define ADD_INST(gen, ...) (gen)->code.push_back(__VA_ARGS__ )
#define ADD_INST_MV(gen, inst) (gen)->code.push_back(std::move(inst))
#define FREE_NODE(gen, node) freeNode((gen)->allocator, (node))
#define FREE_NODE_REC(gen, node) freeRecursive((gen)->allocator, (node))
// symbol table variable translation
void translateFunction(CodeGenerator* gen, AstNode* parseTree);
void translateDeclaration(CodeGenerator* gen, AstNode* parseTree);
void emitGlobalVariable(CodeGenerator* gen, AstNode* parseTree);
#endif