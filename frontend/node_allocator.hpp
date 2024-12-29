#ifndef NODE_ALLOCATOR_H
#define NODE_ALLOCATOR_H

#include "parser.hpp"
#include <vector>
#include <unordered_set>

struct NodeAllocator
{
    std::unordered_set<AstNode*> nodes;
};

AstNode* allocateNode(NodeAllocator* allocator);
void freeNode(NodeAllocator* allocator, AstNode* node, bool rmType = true, bool rmData = true);
void freeRecursive(NodeAllocator* allocator, AstNode* node);
void freeAllNodes(NodeAllocator* allocator);
#endif