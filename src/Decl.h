#ifndef HEADER_IMPORT
#define HEADER_IMPORT
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b)) 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
struct Cell {
    int16_t row;
    int16_t col;
    int value;
    int associated_const;
    int associated_sum;
    int num_parents;
    int8_t operation;
    bool is_faulty;
    struct Cell *par1;
    struct Cell *par2;
    struct Node *children;
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

struct StackNode {
    struct Cell* cur, *par;
    struct StackNode* next;
    struct Node* start;
};

struct Dfsnode {
    struct Cell* cur;
    struct Dfsnode* next;
    struct Node* start;
};

struct Node {
    struct Node * next;
    struct Cell * key;
};

struct LinkedListNode {
    struct Cell * value;
    struct Cell * par_cell;
    struct LinkedListNode * next;
};

#endif