#ifndef NODE_ALLOCATOR_H
#define NODE_ALLOCATOR_H

#include "parser.hpp"
#include <vector>

AstNode* allocateNode();
void freeNode(AstNode* node);
void freeAllNodes(std::vector<AstNode*>* nodes);
#endif