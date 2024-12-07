#include "node_allocator.hpp"

AstNode *allocateNode()
{
    return new AstNode();
}

void freeNode(AstNode *node)
{
    delete node;
}

void freeAllNodes(std::vector<AstNode*>* nodes)
{
    for(AstNode* node : *nodes)
    {
        free(node);
    }
}
