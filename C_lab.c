#include "Decl.h"
#include "AVL.h"
#include "Eval.h"
#include "Display.h"
#include "input_parser.h"

int main(int argc, char * argv[]){
    clock_t in_start_time = clock();
    int R = atoi(argv[1]); int C = atoi(argv[2]);
    struct Cell ** spreadsheet = (struct Cell **) calloc(R, sizeof(struct Cell *));
    for(int i = 0; i<R; i++){
        spreadsheet[i] = (struct Cell *) calloc(C, sizeof(struct Cell));
    }
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            spreadsheet[i][j].row = i;
            spreadsheet[i][j].col = j;
            spreadsheet[i][j].value = 0;
            spreadsheet[i][j].par2 = spreadsheet[i][j].par1 = NULL;
            spreadsheet[i][j].children = NULL;
        }
    }
    int cur_start_row = 0, cur_start_col = 0;
    int cur_end_row = min(9, R-1), cur_end_col = min(9, C-1);
    int constant;
    int cell_ix;
    int cell_iy;
    int cell_1x;
    int cell_1y;
    int cell_2x;
    int cell_2y;
    int operation, prevoprow, prevopcol;
    printer(cur_start_row, cur_end_row, cur_start_col, cur_end_col, spreadsheet);
    printf("[%.2f] (ok) > ", (double)(clock() - in_start_time) / CLOCKS_PER_SEC);
    bool suppress_output = false;
    while (true) {
        char inp[30];
        bool result = false;
        fgets(inp, sizeof(inp), stdin);
        clock_t start_time = clock();
        if (is_valid_input(inp, R, C) == 1) {
            // printf("%s m\n", inp);
            if (strlen(inp) == 1) {
                if (inp[0] == 'Q') break;
                else if (inp[0] == 'W') {
                    if (cur_start_row - 10 < 0) {
                        if (prevoprow != SCROLL) {
                            cur_start_row = 0;
                            cur_end_row = min(R - 1, 9); 
                        } else {
                            cur_start_row = 0;
                        }
                    } else {
                        if (prevoprow != SCROLL) { // completely unrolled in gaps of 10
                            cur_start_row = cur_start_row - 10;
                            cur_end_row = cur_end_row - 10;
                        } else {
                            prevoprow = 0;
                            cur_start_row = cur_start_row - 10;
                            cur_end_row = cur_end_row - 1;
                        }
                    }
                }
                else if (inp[0] == 'S') {
                    if (cur_end_row + 10 >= R) {
                        if (prevoprow != SCROLL) {
                            cur_end_row = R - 1;
                            cur_start_row = max(0, R - 10);
                        } else {
                            cur_end_row = R - 1;
                        }
                    } else {
                        if (prevoprow != SCROLL) {
                            cur_end_row = cur_end_row + 10;
                            cur_start_row = cur_start_row + 10;
                        } else {
                            prevoprow = 0;
                            cur_end_row = cur_end_row + 10;
                            cur_start_row = cur_start_row + 1;
                        }
                    }
                }
                else if (inp[0] == 'A') {
                    if (cur_start_col - 10 < 0) {
                        if (prevopcol != SCROLL) {
                            cur_start_col = 0;
                            cur_end_col = min(C - 1, 9);
                        } else {
                            cur_start_col = 0;
                        }
                    } else {
                        if (prevopcol != SCROLL) {
                            cur_start_col = cur_start_col - 10;
                            cur_end_col = cur_end_col - 10;
                        } else {
                            prevopcol = 0;
                            cur_start_col = cur_start_col - 10;
                            cur_end_col = cur_end_col - 1;
                        }
                    }
                }
                else if (inp[0] == 'D') {
                    if (cur_end_col + 10 >= C) {
                        if (prevopcol != SCROLL) {
                            cur_end_col = C - 1;
                            cur_start_col = max(0, C - 10);
                        } else {
                            cur_end_col = C - 1;
                        }
                    } else {
                        if (prevopcol != SCROLL) {
                            cur_end_col = cur_end_col + 10;
                            cur_start_col = cur_start_col + 10;
                        } else {
                            prevopcol = 0;
                            cur_end_col = cur_end_col + 10;
                            cur_start_col = cur_start_col + 1;
                        }
                    }
                }
                if (!suppress_output)
                printer(cur_start_row, cur_end_row, cur_start_col, cur_end_col, spreadsheet);
                printf("[%.2f] (ok) > ", (double)(clock() - start_time) / CLOCKS_PER_SEC);
            }
            // disable_output and stuff
            else if (strcmp("DISABLE_OUTPUT", inp) == 0) {
                suppress_output = true;
                printf("[%.2f] (ok) > ", (double)(clock() - start_time) / CLOCKS_PER_SEC);
            } else if (strcmp("ENABLE_OUTPUT", inp) == 0) {
                suppress_output = false;
                printer(cur_start_row, cur_end_row, cur_start_col, cur_end_col, spreadsheet);
                printf("[%.2f] (ok) > ", (double)(clock() - start_time) / CLOCKS_PER_SEC);
            } else {
                parse_input(inp, &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation);
                cell_ix = max(0, cell_ix - 1);
                cell_iy = max(0, cell_iy - 1);
                cell_1y = max(0, cell_1y - 1);
                cell_1x = max(0, cell_1x - 1);
                cell_2x = max(0, cell_2x - 1);
                cell_2y = max(0, cell_2y - 1);
                // printf("%s %d %d %d\n", inp, cell_ix, cell_iy, operation);
                if (operation == SCROLL) {
                    prevoprow = prevopcol = SCROLL;
                    cur_start_col = cur_end_col = cell_ix;
                    cur_start_row = cur_end_row = cell_iy;
                }
                else {
                    // printf("HELLO\n");
                    result = update_cell(&spreadsheet[cell_iy][cell_ix], spreadsheet, &spreadsheet[cell_1y][cell_1x], &spreadsheet[cell_2y][cell_2x], constant, operation, R, C);
                }
                if (!suppress_output)
                printer(cur_start_row, cur_end_row, cur_start_col, cur_end_col, spreadsheet);
                if (result || operation == SCROLL) {
                    printf("[%.2f] (ok) > ", (double)(clock() - start_time) / CLOCKS_PER_SEC);
                } else {
                    printf("[%.2f] (Cyclic dependency) > ", (double)(clock() - start_time) / CLOCKS_PER_SEC);
                }
            }
        } else if (is_valid_input(inp, R, C) == 0) {
            // printf("%s %d %d %d\n", inp, cell_ix, cell_iy, operation);
            if (!suppress_output) printer(cur_start_row, cur_end_row, cur_start_col, cur_end_col, spreadsheet);
                printf("[%.2f] (unrecognized cmd) > ", (double)(clock() - start_time) / CLOCKS_PER_SEC);
        }
    }
    return 0;
}

