#include "Decl.h"

void columnNumberToName(int columnNumber, char *result) {
    int index = 0;
    while (columnNumber > 0) {
        columnNumber--;  // Adjust for 0-based index
        result[index++] = (columnNumber % 26) + 'A';
        columnNumber /= 26;
    }
    result[index] = '\0';

    // Reverse the result to get the correct order
    for (int i = 0; i < index / 2; i++) {
        char temp = result[i];
        result[i] = result[index - i - 1];
        result[index - i - 1] = temp;
    }
}

void printer(int rowstart, int rowend, int colstart, int colend, struct Cell** spreadsheet) {
    // 1 - A, 26 - Z, 26 + 1 - AA, 26*2 - AZ, 26*2 + 1 - BA, 26*3 - BZ, 26*26 - YZ, 26*26 + 26 - ZZ, 26*26 + 26 + 1 - AAA, 26*26 + 26*26 + 26 - AZZ
    char result[10]; // Buffer for Excel column name
    printf("   ");
    for (int i = colstart; i <= colend; i++) {
        columnNumberToName(i + 1, result);
        printf(" %11s", result);
    }
    printf("\n");
    for (int i = rowstart; i <= rowend; i++) {
        printf("%3d", i + 1);
        for (int j = colstart; j <= colend; j++) {
            if (spreadsheet[i][j].is_faulty) {
                char *arr = "ERR";
                printf(" %11s", arr);
            } else
            printf(" %11d", spreadsheet[i][j].value);
        }
        printf("\n");
    }
}
