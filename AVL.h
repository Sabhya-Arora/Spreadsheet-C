#ifndef AVL_H
#define AVL_H

#include "Decl.h"

struct Node { 
    struct Node *left;
    struct Node *right;
    struct Cell *key;
    int height;
};

// Function declarations
bool comp(struct Cell *a, struct Cell *b);
struct Node* createNode(struct Cell *key);
struct Node* insert(struct Node* node, struct Cell* key);
struct Node* deleteNode(struct Node* root, struct Cell* key);
void remove_cell(struct Node **avl, struct Cell *key);
void add_cell(struct Node **avl, struct Cell *key);
int getHeight(struct Node* n);
int getBalanceFactor(struct Node* n);
struct Node* rightRotate(struct Node* y);
struct Node* leftRotate(struct Node* x);
struct Node* minValueNode(struct Node* node);

#endif
