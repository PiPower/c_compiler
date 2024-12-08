#include "node_allocator.hpp"

AstNode *allocateNode(NodeAllocator* allocator)
{
    AstNode* node = new AstNode();
    allocator->nodes.insert(node);
    node->nodeType = NodeType::NONE;
    return node;
}

void freeNode(NodeAllocator* allocator, AstNode *node)
{
    allocator->nodes.erase(node);
    delete node;
}

void freeAllNodes(NodeAllocator* allocator)
{
    for(AstNode* node : allocator->nodes)
    {
        freeNode(allocator, node);
    }
}
