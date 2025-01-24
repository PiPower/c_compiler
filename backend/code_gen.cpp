#include "code_gen.hpp"
#include "code_gen_internal.hpp"
#include "cpu.hpp"
#include <string.h>
using namespace std;

void generate_code(CodeGenerator *gen, std::vector<AstNode*>* globalTrees)
{
    fillTypeHwdInfoForBlock(gen->symtab);
    for (size_t i = 0; i < globalTrees->size(); i++)
    {
        AstNode* parseTree = (*globalTrees)[i];
        dispatch(gen, parseTree);
    }
    
    return;
}

void dispatch(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::FUNCTION_DEF:
        translateFunction(gen, parseTree);
        break;
    case NodeType::CONSTANT:
        prepareConstant(gen, parseTree);
        break;
    case NodeType::IDENTIFIER:
        if(parseTree->children.size() == 0 && gen->localSymtab->scopeLevel != 0)
        {
            prepareVariable(gen, parseTree);
        }
        else
        {
            translateDeclaration(gen, parseTree);
        }
        break;
    default:
        translateExpr(gen, parseTree);
        break;
    }
}

void write_to_file(const InstructionSeq& instructions, 
        const std::map<uint64_t, std::string>& floatConsts, FILE* stream)
{
    for(const Instruction& inst : instructions)
    {
        if( inst.type == LABEL)
        {
            write_label(&inst, stream);
        }
        else if(inst.type == INSTRUCTION)
        {
            write_instruction(&inst, stream);
        }
    }
    auto constIter = floatConsts.begin();
    while (constIter != floatConsts.end())
    {
        int lo, hi;
        memcpy(&lo, &constIter->first, sizeof(int));
        memcpy(&hi, (char*)(&constIter->first) + 4, sizeof(int));
        if(hi != 0 )
        {
            fprintf(stream, ".section .rodata \n.align 8\n");
        }
        else
        {
            fprintf(stream, ".section .rodata \n.align 4\n");
        }

        fprintf(stream, "%s:\n\t.long %d\n", constIter->second.c_str(), lo);
        if(hi != 0 )
        {
            fprintf(stream, "\t.long %d\n", hi);
        }
        advance(constIter, 1);
    }
    
}

void write_label(const Instruction *inst, FILE *stream)
{
    size_t offset = 0;
    while (offset < inst->src.size())
    {
        offset += fprintf(stream, "\t%s\n", inst->src.c_str() + offset);
        offset += 1 - 2; // null_char - \t - \n 
    }
    fprintf(stream, "%s:\n", inst->mnemonic.c_str());
    
    offset = 0;
    while (offset < inst->dest.size())
    {
        offset += fprintf(stream, "\t%s\n", inst->dest.c_str() + offset);
        offset += 1 - 2; // null_char - \t - \n 
    }
}

void write_instruction(const Instruction *inst, FILE *stream)
{
    fprintf(stream, "\t%s ", inst->mnemonic.c_str());
    if(inst->src.size() > 0)
    {
        fprintf(stream, "%s", inst->src.c_str());
    }
    if(inst->dest.size() > 0)
    {
        fprintf(stream, ", %s", inst->dest.c_str());
    }
    fprintf(stream, "\n");
}
