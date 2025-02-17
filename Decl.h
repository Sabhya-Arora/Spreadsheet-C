#include <stdbool.h>
#ifndef HEADER_IMPORT
#define HEADER_IMPORT
struct Cell{
    int row;
    int col;
    int value;
    int operation;
    int num_parents;
    int associated_const;
    int associated_sum;
    int associated_sq_sum;
    int associated_n;
    bool is_faulty;
    struct Cell * par1;
    struct Cell * par2;
    struct Node * children;
};
enum ops {
    CONST = 1,
    SINGLE_CELL,
    CELL_ADD_CONST,
    CELL_MULT_CONST,
    CELL_DIV_CONST,
    CONST_DIV_CELL,
    CONST_SUB_CELL,
    CELL_ADD_CELL,
    CELL_SUB_CELL,
    CELL_MULT_CELL,
    CELL_DIV_CELL,
    MAX,
    MIN,
    SUM,
    AVG,
    STD_DEV,
    SLEEP_CONST,
    SLEEP_CELL,
    SCROLL,
    ERR
};
struct LinkedListNode {
    struct Cell * value;
    struct Cell * par_cell;
    struct LinkedListNode * next;
};



#endif