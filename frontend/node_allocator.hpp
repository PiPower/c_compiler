#ifndef NODE_ALLOCATOR_H
#define NODE_ALLOCATOR_H

#include "parser.hpp"

AstNode* allocateNode();
void freeNode(AstNode* node);

#endif