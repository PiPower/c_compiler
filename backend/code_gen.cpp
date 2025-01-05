#include "code_gen.hpp"
#include "code_gen_internal.hpp"
using namespace std;

char* generate_code(CodeGenerator *gen)
{
    for (size_t i = 0; i < gen->parseTrees->size(); i++)
    {
        AstNode* parseTree = (*gen->parseTrees)[i];
        dispatcher(gen, parseTree);
    }
    
    return nullptr;
}

void dispatcher(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::FUNCTION_DEF:
        translateFunction(gen, parseTree);
        break;
    case NodeType::IDENTIFIER:
        translateDeclaration(gen, parseTree);
        break;
    
    default:
        break;
    }
}

void write_to_file(const InstructionSeq &instructions, FILE* stream)
{
    for(const Instruction& inst : instructions)
    {
        if( inst.type == LABEL)
        {
            write_label(&inst, stream);
        }
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
