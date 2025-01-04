#ifndef CODE_GEN_INTERNAL
#define CODE_GEN_INTERNAL
#include "code_gen.hpp"

struct FunctionLayout
{
    /* data */
};
#define GET_SCOPED_SYM(gen, name)  getSymbol((gen)->scopedSymtab, (name)) 
#define ADD_INST_MV(gen, inst) (gen)->code.push_back(std::move(inst))
// symbol table variable translation
void translateFunction(CodeGenerator* gen, AstNode* parseTree);
void translateDeclaration(CodeGenerator* gen, AstNode* parseTree);
void emitGlobalVariable(CodeGenerator* gen, AstNode* parseTree);
#endif