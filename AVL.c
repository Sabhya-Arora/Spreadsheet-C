#ifndef AVL_IMPORTED
#define AVL_IMPORTED
#include "Decl.h"
struct AVL;
struct Node;
struct Cell;

bool comp(struct Cell * a, struct Cell * b ){
    if(a->row < b->row) return true;
    if(a->row > b->row) return false;
    if (a->col < b->col) return true;
    return false;
}
struct Node {
    struct Node * left;
    struct Node * right;
    struct Cell * key;
    int height;
};
int getHeight(struct Node* n)
{
    if (n == NULL)
        return 0;
    return n->height;
}
int getBalanceFactor(struct Node* n){
    if (n == NULL)
        return 0;
    return getHeight(n->left) - getHeight(n->right);
}
struct Node* createNode(struct Cell * key){
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // New node is initially added at leaf
    return node;
}
struct Node* rightRotate(struct Node* y){
    struct Node* x = y->left;
    struct Node* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}
struct Node* leftRotate(struct Node* x){
    struct Node* y = x->right;
    struct Node* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}
struct Node* insert(struct Node* node, struct Cell * key){

    if (node == NULL)
        return createNode(key);

    if (comp (key, node->key)) node->left = insert(node->left, key);
    else node->right = insert(node->right, key);

    // 2. Update height of this ancestor node
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    int balance = getBalanceFactor(node);

    if (balance > 1){
        if (comp(key, node->left->key))
            return rightRotate(node);
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 )
        if(comp(node->right->key, key))
            return leftRotate(node);
        node->right = rightRotate(node->right);
        return leftRotate(node);


    return node;
}
struct Node * minValueNode(struct Node* node)
{
    struct Node* current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}
struct Node* deleteNode(struct Node* root, struct Cell * key)
{

    if (root == NULL)
        return root;

    if ( comp(key, root->key) )
        root->left = deleteNode(root->left, key);

    else if(comp(root->key, key) )
        root->right = deleteNode(root->right, key);

    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) ){
            struct Node *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL){
                temp = root;
                root = NULL;
            }
            else *root = *temp; 
            free(temp);
        }
        else
        {
            struct Node* temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = deleteNode(root->right, temp->key);
        }
    }

    // If the tree had only one node then return
    if (root == NULL)
      return root;

    root->height = 1 + max(getHeight(root->left), getHeight(root->right));
    int balance = getBalanceFactor(root);

    if (balance > 1 )
        if(getBalanceFactor(root->left) >= 0)
            return rightRotate(root);
        root->left =  leftRotate(root->left);
        return rightRotate(root);

    if (balance < -1 )
        if(getBalanceFactor(root->right) <=0)
            return leftRotate(root);
        root->right = rightRotate(root->right);
        return leftRotate(root);


    return root;
}

void remove_cell (struct Node ** avl, struct Cell * key){
    *avl = deleteNode(*avl, key);
}
void add_cell (struct Node ** avl, struct Cell * key){
    *avl= insert(*avl, key);
}


#endif