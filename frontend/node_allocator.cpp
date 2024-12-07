#include "node_allocator.hpp"

AstNode *allocateNode()
{
    return new AstNode();
}

void freeNode(AstNode *node)
{
    delete node;
}
