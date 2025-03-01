#ifndef EVAL_H
#define EVAL_H

#include "Decl.h"

void calc(struct Cell *current, struct Cell **spreadsheet);
void recalc(struct Cell *current, struct Cell *par_cell, int old_value, bool was_faulty, struct Cell **spreadsheet);
bool cycle_detect(struct Cell* current, struct Cell** spreadsheet, int rows, int cols, struct Cell *tobepar1, struct Cell* tobepar2, int operation);
bool update_cell(struct Cell* current, struct Cell **spreadsheet, struct Cell*parent1, struct Cell*parent2, int constant, int operation, int totalrow, int totalcol);

#endif
