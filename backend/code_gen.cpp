#include "code_gen.hpp"
#include "code_gen_internal.hpp"

char* generate_code(CodeGenerator *gen)
{

    for (size_t i = 0; i < gen->parseTrees->size(); i++)
    {
        AstNode* parseTree = (*gen->parseTrees)[i];
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
    
    default:
        break;
    }
}
