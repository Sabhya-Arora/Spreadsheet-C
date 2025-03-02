#include <check.h>
#include "../src/Eval.h"  // Include the source file (or replace with a header)
#include "../src/AVL.h"
#include "../src/Decl.h"
#include "../src/input_parser.h"
#include "../src/Display.h"

START_TEST(test_columnNumberToName)
{
    char result[10];

    columnNumberToName(1, result);
    ck_assert_str_eq(result, "A");

    columnNumberToName(26, result);
    ck_assert_str_eq(result, "Z");

    columnNumberToName(27, result);
    ck_assert_str_eq(result, "AA");

    columnNumberToName(52, result);
    ck_assert_str_eq(result, "AZ");

    columnNumberToName(53, result);
    ck_assert_str_eq(result, "BA");

    columnNumberToName(702, result);
    ck_assert_str_eq(result, "ZZ");

    columnNumberToName(703, result);
    ck_assert_str_eq(result, "AAA");
}
END_TEST

START_TEST(test_printer)
{
    int R = 5, C = 5;
    struct Cell spreadsheet[R][C];

    // Initialize spreadsheet with test values
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            spreadsheet[i][j].value = (i + 1) * 10 + (j + 1);
            spreadsheet[i][j].is_faulty = 0;
        }
    }

    // Set a faulty cell
    spreadsheet[2][3].is_faulty = 1; // Cell C3 should print "ERR"

    // Redirect output to file
    FILE *f = fopen("build/tests/test_output.txt", "w");
    ck_assert_ptr_nonnull(f);
    freopen("build/tests/test_output.txt", "w", stdout);

    // Run printer function
    struct Cell *spreadsheet_ptrs[R];
    for (int i = 0; i < R; i++) {
        spreadsheet_ptrs[i] = spreadsheet[i];
    }
    printer(0, 0, spreadsheet_ptrs, R, C);

    fclose(f);
    freopen("/dev/tty", "w", stdout); // Reset stdout to terminal

    // Open output file for reading
    FILE *out = fopen("build/tests/test_output.txt", "r");
    ck_assert_ptr_nonnull(out);

    // Expected numeric values
    int expected_values[5][5] = {
        {11, 12, 13, 14, 15},
        {21, 22, 23, 24, 25},
        {31, 32, 33, -1, 35}, // -1 represents "ERR"
        {41, 42, 43, 44, 45},
        {51, 52, 53, 54, 55}
    };

    char buffer[256];
    int row = -1, col = 0;
    
    // Read file line by line
    while (fgets(buffer, sizeof(buffer), out)) {
        if (isdigit(buffer[0])) {  // Ignore header row (A, B, C...)
            row++;
            col = 0;
            char *token = strtok(buffer, " \t\n"); // Tokenize by space/tab
            while (token != NULL) {
                if (col > 0) {  // Ignore row number column
                    if (strcmp(token, "ERR") == 0) {
                        ck_assert_int_eq(expected_values[row][col - 1], -1);
                    } else {
                        int value = atoi(token);
                        ck_assert_int_eq(value, expected_values[row][col - 1]);
                    }
                }
                token = strtok(NULL, " \t\n");
                col++;
            }
        }
    }

    fclose(out);
}
END_TEST

START_TEST(test_to_uppercase) {
    char str1[] = "hello";
    to_uppercase(str1);
    ck_assert_str_eq(str1, "HELLO");

    char str2[] = "123abcXYZ";
    to_uppercase(str2);
    ck_assert_str_eq(str2, "123ABCXYZ");
}
END_TEST

START_TEST(test_remove_spaces) {
    char str1[] = "  hello  world ";
    remove_spaces(str1);
    ck_assert_str_eq(str1, "helloworld");

    char str2[] = "\n A B C \n";
    remove_spaces(str2);
    ck_assert_str_eq(str2, "ABC");
}
END_TEST

START_TEST(test_column_to_number) {
    ck_assert_int_eq(column_to_number("A"), 1);
    ck_assert_int_eq(column_to_number("Z"), 26);
    ck_assert_int_eq(column_to_number("AA"), 27);
    ck_assert_int_eq(column_to_number("AZ"), 52);
    ck_assert_int_eq(column_to_number("BA"), 53);
    ck_assert_int_eq(column_to_number("ZZ"), 702);
    ck_assert_int_eq(column_to_number("AAA"), 703);
}
END_TEST

START_TEST(test_is_valid_cell) {
    ck_assert_int_eq(is_valid_cell("A1", 10, 10), 1);
    ck_assert_int_eq(is_valid_cell("Z99", 100, 26), 1);
    ck_assert_int_eq(is_valid_cell("AA10", 50, 27), 1);
    ck_assert_int_eq(is_valid_cell("B0", 10, 10), 0);   // Invalid row number
    ck_assert_int_eq(is_valid_cell("A11", 10, 10), 0);  // Row exceeds range
    ck_assert_int_eq(is_valid_cell("ZZ99", 50, 26), 0); // Column exceeds range
}
END_TEST

START_TEST(test_is_valid_constant) {
    ck_assert_int_eq(is_valid_constant("123"), 1);
    ck_assert_int_eq(is_valid_constant("+456"), 1);
    ck_assert_int_eq(is_valid_constant("-789"), 1);
    ck_assert_int_eq(is_valid_constant("12a3"), 0);  // Invalid
    ck_assert_int_eq(is_valid_constant("--123"), 0); // Invalid
}
END_TEST

START_TEST(test_is_valid_formula) {
    ck_assert_int_eq(is_valid_formula("A1=10", 10, 10), 1);
    ck_assert_int_eq(is_valid_formula("B2=A1+5", 10, 10), 1);
    ck_assert_int_eq(is_valid_formula("C3=SUM(A1:B2)", 10, 10), 1);
    ck_assert_int_eq(is_valid_formula("D4=MAX(A1:Z99)", 100, 26), 1);
    ck_assert_int_eq(is_valid_formula("E5=A1-", 10, 10), 0); // Invalid formula
    ck_assert_int_eq(is_valid_formula("F6==10", 10, 10), 0); // Invalid syntax
}
END_TEST
START_TEST(test_is_valid_range) {
    ck_assert_int_eq(is_valid_range("(A1:B2)", 10, 10), 1);  // Valid range
    ck_assert_int_eq(is_valid_range("(C3:D4)", 10, 10), 1);  // Valid range
    ck_assert_int_eq(is_valid_range("(Z1:AA2)", 100, 27), 1); // Valid range with multi-letter columns

    ck_assert_int_eq(is_valid_range("(B2:A1)", 10, 10), 0);  // Invalid: first cell should be <= second cell
    ck_assert_int_eq(is_valid_range("(A1:Z99)", 50, 26), 0); // Out of bounds

    ck_assert_int_eq(is_valid_range("(A1:)", 10, 10), 0);  // Missing second cell
    ck_assert_int_eq(is_valid_range("(:B2)", 10, 10), 0);  // Missing first cell
    ck_assert_int_eq(is_valid_range("(A1B2)", 10, 10), 0); // Missing colon
    ck_assert_int_eq(is_valid_range("(A1:B2", 10, 10), 0); // Missing closing parenthesis
    ck_assert_int_eq(is_valid_range("A1:B2)", 10, 10), 0); // Missing opening parenthesis
    ck_assert_int_eq(is_valid_range("", 10, 10), 0);       // Empty string
    ck_assert_int_eq(is_valid_range(NULL, 10, 10), 0);     // NULL input
}
END_TEST
START_TEST(test_is_valid_input) {
    int R = 10, C = 10; // Example grid size

    // Valid single-character commands
    ck_assert_int_eq(is_valid_input("W", R, C), 1);
    ck_assert_int_eq(is_valid_input("S", R, C), 1);
    ck_assert_int_eq(is_valid_input("A", R, C), 1);
    ck_assert_int_eq(is_valid_input("D", R, C), 1);
    ck_assert_int_eq(is_valid_input("Q", R, C), 1);

    // Valid commands with extra spaces (should be handled by remove_spaces)
    ck_assert_int_eq(is_valid_input("  W ", R, C), 1);
    ck_assert_int_eq(is_valid_input("  scroll_to  A1 ", R, C), 1);

    // Valid cell reference commands
    ck_assert_int_eq(is_valid_input("SCROLL_TO A1", R, C), 1);
    ck_assert_int_eq(is_valid_input("SCROLL_TO Z9", R, C), 1);
    ck_assert_int_eq(is_valid_input("SCROLL_TO AA1", R, C), 1);

    // Valid enable/disable output commands
    ck_assert_int_eq(is_valid_input("ENABLE_OUTPUT", R, C), 1);
    ck_assert_int_eq(is_valid_input("DISABLE_OUTPUT", R, C), 1);

    // Valid formulas
    ck_assert_int_eq(is_valid_input("A1=10", R, C), 1);
    ck_assert_int_eq(is_valid_input("B2=A1+5", R, C), 1);
    ck_assert_int_eq(is_valid_input("C3=SUM(A1:B2)", R, C), 1);
    ck_assert_int_eq(is_valid_input("D4=MAX(A1:B10)", R, C), 1);

    // Invalid inputs
    ck_assert_int_eq(is_valid_input("X", R, C), 0); // Invalid command
    ck_assert_int_eq(is_valid_input("SCROLL_TO", R, C), 0); // Missing argument
    ck_assert_int_eq(is_valid_input("SCROLL_TO A11", R, C), 0); // Out of range
    ck_assert_int_eq(is_valid_input("ENABLE_OUTPUTS", R, C), 0); // Typo in command
    ck_assert_int_eq(is_valid_input("DISABLE", R, C), 0); // Incomplete command
    ck_assert_int_eq(is_valid_input("A1==10", R, C), 0); // Invalid formula
    ck_assert_int_eq(is_valid_input("B2=10+", R, C), 0); // Incomplete formula
    ck_assert_int_eq(is_valid_input("", R, C), 0); // Empty string
    ck_assert_int_eq(is_valid_input(NULL, R, C), 0); // NULL input
}
END_TEST

START_TEST(test_parse_input) {
    int constant, cell_ix, cell_iy, cell_1x, cell_1y, cell_2x, cell_2y, operation;

    // Test constant input
    ck_assert_int_eq(parse_input("A1=5", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(constant, 5);
    ck_assert_int_eq(operation, CONST);

    // Test basic cell assignment
    ck_assert_int_eq(parse_input("B2=A1", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, SINGLE_CELL);
    ck_assert_int_eq(cell_1x, 1); // Column A → index 1
    ck_assert_int_eq(cell_1y, 1); // Row 1 → index 1

    // Test addition operation
    ck_assert_int_eq(parse_input("C3=A1+10", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, CELL_ADD_CONST);
    ck_assert_int_eq(cell_1x, 1);
    ck_assert_int_eq(cell_1y, 1);
    ck_assert_int_eq(constant, 10);

    // Test multiplication operation
    ck_assert_int_eq(parse_input("D4=B2*C3", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, CELL_MULT_CELL);
    ck_assert_int_eq(cell_1x, 2);
    ck_assert_int_eq(cell_1y, 2);
    ck_assert_int_eq(cell_2x, 3);
    ck_assert_int_eq(cell_2y, 3);

    // Test division operation with a constant
    ck_assert_int_eq(parse_input("E5=100/5", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, CONST);
    ck_assert_int_eq(constant, 20);

    // Test SUM function
    ck_assert_int_eq(parse_input("F6=SUM(A1:B2)", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, SUM);
    ck_assert_int_eq(cell_1x, 1);
    ck_assert_int_eq(cell_1y, 1);
    ck_assert_int_eq(cell_2x, 2);
    ck_assert_int_eq(cell_2y, 2);

    // Test MIN function
    ck_assert_int_eq(parse_input("G7=MIN(A1:B2)", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, MIN);

    // Test SCROLL_TO function
    ck_assert_int_eq(parse_input("SCROLL_TO H8", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, SCROLL);
    ck_assert_int_eq(cell_ix, 8); // Column H → index 8
    ck_assert_int_eq(cell_iy, 8); // Row 8 → index 8

    // Test SLEEP function with a constant value
ck_assert_int_eq(parse_input("A1=SLEEP(5)", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
ck_assert_int_eq(operation, SLEEP_CONST);
ck_assert_int_eq(constant, 5);
ck_assert_int_eq(cell_ix, 1);  // Column A → index 1
ck_assert_int_eq(cell_iy, 1);  // Row 1 → index 1

// Test SLEEP function with a cell reference
ck_assert_int_eq(parse_input("B2=SLEEP(C3)", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
ck_assert_int_eq(operation, SLEEP_CELL);
ck_assert_int_eq(cell_1x, 3);  // Column C → index 3
ck_assert_int_eq(cell_1y, 3);  // Row 3 → index 3
ck_assert_int_eq(cell_ix, 2);  // Column B → index 2
ck_assert_int_eq(cell_iy, 2);  // Row 2 → index 2


    // Test invalid input
    ck_assert_int_eq(parse_input("INVALID=100", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 0);
    ck_assert_int_eq(parse_input("A1=100/0", &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation), 1);
    ck_assert_int_eq(operation, ERR); // Division by zero should return an error

}
END_TEST
START_TEST(test_cycle_detect) {
    int rows = 3, cols = 3;
    struct Cell **spreadsheet = malloc(rows * sizeof(struct Cell *));
    for (int i = 0; i < rows; i++) {
        spreadsheet[i] = calloc(cols, sizeof(struct Cell));
    }

    struct Cell *A1 = &spreadsheet[0][0];
    struct Cell *B1 = &spreadsheet[0][1];
    struct Cell *C1 = &spreadsheet[0][2];

    A1->row = 0; A1->col = 0;
    B1->row = 0; B1->col = 1;
    C1->row = 0; C1->col = 2;

    // A1 = B1 + 10 (No cycle)
    ck_assert_int_eq(cycle_detect(A1, spreadsheet, rows, cols, B1, NULL, CELL_ADD_CONST), 0);

    // A1 = A1 + 10 (Direct self-referential cycle)
    ck_assert_int_eq(cycle_detect(A1, spreadsheet, rows, cols, A1, NULL, CELL_ADD_CONST), 1);

    // A1 → B1 → C1 → A1 (Indirect cycle)
    B1->par1 = A1;
    add_cell(&(A1->children), B1);
    C1->par1 = B1;
    add_cell(&(B1->children), C1);
    ck_assert_int_eq(cycle_detect(A1, spreadsheet, rows, cols, C1, NULL, CELL_ADD_CONST), 1);

    // A1 = SUM(A1:B1) (Range-based cycle where A1 is included in the sum)
    ck_assert_int_eq(cycle_detect(A1, spreadsheet, rows, cols, A1, B1, SUM), 1);

    for (int i = 0; i < rows; i++) {
        free(spreadsheet[i]);
    }
    free(spreadsheet);
}
END_TEST


START_TEST(test_toposort) {
    int rows = 3, cols = 3;
    struct Cell **spreadsheet = malloc(rows * sizeof(struct Cell *));
    for (int i = 0; i < rows; i++) {
        spreadsheet[i] = calloc(cols, sizeof(struct Cell));
    }

    struct Cell *A1 = &spreadsheet[0][0];
    struct Cell *B1 = &spreadsheet[0][1];
    struct Cell *C1 = &spreadsheet[0][2];

    A1->row = 0; A1->col = 0;
    B1->row = 0; B1->col = 1;
    C1->row = 0; C1->col = 2;

    B1->par1 = A1;
    C1->par1 = B1;

    struct LinkedListNode *topos = NULL;
    bool **visited = malloc(rows * sizeof(bool *));
    for (int i = 0; i < rows; i++) {
        visited[i] = calloc(cols, sizeof(bool));
    }

    toposort(A1, A1, &topos, visited);
    ck_assert_ptr_nonnull(topos);
    ck_assert_ptr_eq(topos->value, A1);

    toposort(A1, B1, &topos, visited);
    ck_assert_ptr_eq(topos->value, B1);

    toposort(A1, C1, &topos, visited);
    ck_assert_ptr_eq(topos->value, C1);

    struct LinkedListNode *cur = topos;
    int count = 0;
    while (cur) {
        count++;
        cur = cur->next;
    }
    ck_assert_int_eq(count, 3);

    for (int i = 0; i < rows; i++) {
        free(visited[i]);
        free(spreadsheet[i]);
    }
    free(visited);
    free(spreadsheet);
}
END_TEST

START_TEST(test_calc) {
    int rows = 3, cols = 3;
    struct Cell **spreadsheet = malloc(rows * sizeof(struct Cell *));
    for (int i = 0; i < rows; i++) {
        spreadsheet[i] = calloc(cols, sizeof(struct Cell));
    }

    struct Cell *A1 = &spreadsheet[0][0];
    struct Cell *B1 = &spreadsheet[0][1];
    struct Cell *C1 = &spreadsheet[0][2];

    A1->row = 0; A1->col = 0;
    B1->row = 0; B1->col = 1;
    C1->row = 0; C1->col = 2;

    A1->operation = CONST;
    A1->value = 10;
    calc(A1, spreadsheet);
    ck_assert_int_eq(A1->value, 10);

    B1->operation = SINGLE_CELL;
    B1->par1 = A1;
    calc(B1, spreadsheet);
    ck_assert_int_eq(B1->value, 10);

    C1->operation = CELL_ADD_CONST;
    C1->par1 = B1;
    C1->associated_const = 5;
    calc(C1, spreadsheet);
    ck_assert_int_eq(C1->value, 15);

    C1->operation = CELL_MULT_CONST;
    C1->associated_const = 2;
    calc(C1, spreadsheet);
    ck_assert_int_eq(C1->value, 20);

    C1->operation = CELL_DIV_CONST;
    C1->associated_const = 5;
    calc(C1, spreadsheet);
    ck_assert_int_eq(C1->value, 2);

    C1->operation = CONST_DIV_CELL;
    C1->associated_const = 100;
    calc(C1, spreadsheet);
    ck_assert_int_eq(C1->value, 10);

    C1->operation = CONST_SUB_CELL;
    C1->associated_const = 50;
    calc(C1, spreadsheet);
    ck_assert_int_eq(C1->value, 40);

    B1->operation = CELL_ADD_CELL;
    B1->par1 = A1;
    B1->par2 = C1;
    calc(B1, spreadsheet);
    ck_assert_int_eq(B1->value, 50);

    B1->operation = CELL_SUB_CELL;
    calc(B1, spreadsheet);
    ck_assert_int_eq(B1->value, -30);

    B1->operation = CELL_MULT_CELL;
    calc(B1, spreadsheet);
    ck_assert_int_eq(B1->value, 400);

    B1->operation = CELL_DIV_CELL;
    calc(B1, spreadsheet);
    ck_assert_int_eq(B1->value, 0);

    for (int i = 0; i < rows; i++) {
        free(spreadsheet[i]);
    }
    free(spreadsheet);
}
END_TEST

Suite *spreadsheet_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Spreadsheet");
    tc_core = tcase_create("Core");

    // Display
    tcase_add_test(tc_core, test_columnNumberToName);
    tcase_add_test(tc_core, test_printer);

    // Input parser
    tcase_add_test(tc_core, test_to_uppercase);
    tcase_add_test(tc_core, test_remove_spaces);
    tcase_add_test(tc_core, test_column_to_number);
    tcase_add_test(tc_core, test_is_valid_cell);
    tcase_add_test(tc_core, test_is_valid_constant);
    tcase_add_test(tc_core, test_is_valid_formula);
    tcase_add_test(tc_core, test_is_valid_range);
    tcase_add_test(tc_core, test_is_valid_input);
    tcase_add_test(tc_core, test_parse_input);

    // Eval
    tcase_add_test(tc_core, test_cycle_detect);
    tcase_add_test(tc_core, test_toposort);
    tcase_add_test(tc_core, test_calc);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = spreadsheet_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
