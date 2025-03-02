#include "Decl.h"
#include "Stack.h" // Include the stack header file

struct StackNode* push(struct StackNode* top, struct Cell* cur, struct Cell* par) {
    struct StackNode* new_node = (struct StackNode*)malloc(sizeof(struct StackNode));
    new_node -> cur = cur;
    new_node -> start = cur -> children;
    new_node -> par = par;
    new_node -> next = top;
    return new_node;
}

struct StackNode* pop(struct StackNode* top) {
    struct StackNode* newhead = top->next;
    free(top);
    return newhead;
}

struct Dfsnode* dfspush(struct Dfsnode* top, struct Cell* cur) {
    struct Dfsnode* new_node = (struct Dfsnode*)malloc(sizeof(struct Dfsnode));
    new_node -> cur = cur;
    new_node -> start = cur -> children;
    new_node -> next = top;
    return new_node;
}

struct Dfsnode* dfspop(struct Dfsnode* top) {
    struct Dfsnode* newhead = top->next;
    free(top);
    return newhead;
}