#ifndef DISPLAY_H
#define DISPLAY_H

#include "Decl.h"

void columnNumberToName(int columnNumber, char *result);
void printer(int rowstart, int rowend, int colstart, int colend, struct Cell** spreadsheet);

#endif
