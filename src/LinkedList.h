#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include "Decl.h"

struct Node {
    struct Node *next;
    struct Cell *key;
};

struct Node* createNode(struct Cell *key);
struct Node* insert(struct Node* node, struct Cell *key);
struct Node* deleteNode(struct Node* root, struct Cell *key);
void remove_cell(struct Node **ll, struct Cell *key);
void add_cell(struct Node **ll, struct Cell *key);

#endif // LINKEDLIST_H
