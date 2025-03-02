#include "Decl.h"
struct Node;
struct Node {
    struct Node * next;
    struct Cell * key;
};
struct Node* createNode(struct Cell * key){
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->key = key;
    node->next = NULL;
    return node;
}
struct Node* insert(struct Node* node, struct Cell * key){
    struct Node* newnode = createNode(key);
    newnode->next = node;
    return newnode;
}
struct Node* deleteNode(struct Node* root, struct Cell * key)
{
    struct Node * cur = root;
    struct Node * prev = NULL;
    while (cur != NULL && cur->key != key)
    {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL)
        return root;
    else {
        if (prev == NULL) {
            root = cur->next;
            free(cur);
        }
        else {
            prev->next = cur->next;
            free(cur);
        }
    }
    return root;
}

void remove_cell (struct Node ** ll, struct Cell * key){
    *ll = deleteNode(*ll, key);
}
void add_cell (struct Node ** ll, struct Cell * key){
    *ll= insert(*ll, key);
}

