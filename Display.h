#ifndef DISPLAY_H
#define DISPLAY_H

#include "Decl.h"

void columnNumberToName(int columnNumber, char *result);
void printer(int rowstart, int colstart, struct Cell** spreadsheet, int R, int C);

#endif
