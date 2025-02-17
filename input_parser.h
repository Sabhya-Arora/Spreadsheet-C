#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "Decl.h"

int parse_input(const char *input, int *constant, int *cell_ix, int *cell_iy, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y, int *operation);
int is_valid_input(char *input, int R, int C);
#endif