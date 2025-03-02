#ifndef STACK_H
#define STACK_H

#include "Decl.h"

struct StackNode* pop(struct StackNode* top);
struct StackNode* push(struct StackNode* top, struct Cell* cur, struct Cell* par);
struct Dfsnode* dfspop(struct Dfsnode* top);
struct Dfsnode* dfspush(struct Dfsnode* top, struct Cell* cur);

#endif