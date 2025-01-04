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
        if( inst.mnemonic.find(':') != string::npos)
        {
            fprintf(stream, "%s%s\n%s", inst.src.c_str(), inst.mnemonic.c_str(), inst.dest.c_str());
        }
    }
}
