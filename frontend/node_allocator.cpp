#include "node_allocator.hpp"

AstNode *allocateNode(NodeAllocator* allocator)
{
    AstNode* node = new AstNode();
    allocator->nodes.insert(node);
    node->nodeType = NodeType::NONE;
    return node;
}


void freeNode(NodeAllocator* allocator, AstNode *node, bool rmType, bool rmData)
{
    if(rmData && node->data)
    {
        delete node->data;
    }
    if(rmType && node->type)
    {
        delete node->type;
    }
    delete node;
    allocator->nodes.erase(node);
}

void freeAllNodes(NodeAllocator* allocator)
{
    for(AstNode* node : allocator->nodes)
    {
        delete node;
    }
    allocator->nodes.clear();
}
