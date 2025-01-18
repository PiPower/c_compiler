#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"

void translateLocalAssignment(CodeGenerator *gen, AstNode *parseTree)
{
    CLEAR_OP(gen);
    dispatch(gen, parseTree->children[0]->children[0]);

}

void translateExpr(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::MINUS:
        translateNegation(gen, parseTree);
        break;
    }
}

void translateNegation(CodeGenerator *gen, AstNode *parseTree)
{
    CLEAR_OP(gen);
    dispatch(gen, parseTree->children[0]);
    if(gen->opDesc.op == OP::CONSTANT)
    {
        gen->opDesc.operand.insert(0, 1, '-');
    }
}
