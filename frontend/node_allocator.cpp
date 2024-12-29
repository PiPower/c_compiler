#include "node_allocator.hpp"
using namespace std;

AstNode *allocateNode(NodeAllocator* allocator)
{
    AstNode* node = new AstNode();
    allocator->nodes.insert(node);
    node->nodeType = NodeType::NONE;
    node->data = nullptr;
    node->type = nullptr;
    return node;
}


void freeNode(NodeAllocator* allocator, AstNode *node, bool rmType, bool rmData)
{
    if(!node)
    {
        return ;
    }

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

void freeRecursive(NodeAllocator *allocator, AstNode *node)
{
    if(!node)
    {
        return;
    }

    queue<AstNode*> nodes;
    nodes.push(node);
    while (nodes.size() != 0)
    {
        AstNode* node = nodes.front();
        nodes.pop();
        for(AstNode* child : node->children)
        {
            nodes.push(child);
        }
        freeNode(allocator, node);
    }

}

void freeAllNodes(NodeAllocator* allocator)
{
    for(AstNode* node : allocator->nodes)
    {
        delete node;
    }
    allocator->nodes.clear();
}
