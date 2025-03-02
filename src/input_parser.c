#include "Decl.h"

//Validity checker

void to_uppercase(char *str){
    while(*str){
        *str = toupper(*str);
        str++;
    }
}

void remove_spaces(char* s){
    char* d = s;
    do{
        while(*d == ' ' || *d == '\n'){
            ++d;
        }
    } while(*s++ = *d++);
}

//converts column name to column number
int column_to_number(char *col) {
    int result = 0;
    while (*col) {
        if (!isalpha(*col)) break;
        result = result * 26 + (toupper(*col) - 'A' + 1);
        col++;
    }
    return result;
}

//checks if a cell is valid
int is_valid_cell(char *cell, int R, int C) {
    if (cell == NULL || *cell == '\0') return 0;
    char *ptr = cell;
    char col[10], row[10]; 
    int col_len = 0, row_len = 0;
    
    //separate column and row
    while(*ptr && isalpha(*ptr)){
        col[col_len++] = *ptr++;
    }
    col[col_len] = '\0';  
    if (!*ptr) return 0;
    while(*ptr && isdigit(*ptr)){
        row[row_len++] = *ptr++;
    }
    row[row_len] = '\0';
    if (*ptr) return 0;
    if (col_len == 0 || row_len == 0) return 0;

    //check if column and row are within bounds
    int col_num = column_to_number(col);
    if (col_num > C) return 0;
 
    int row_num = atoi(row);
    if (row_num < 1 || row_num > R) return 0;

    return 1; 
}

//check if a constant is valid
int is_valid_constant(char *cst){
    if (cst == NULL || *cst == '\0') return 0;
    char *ptr = cst;
    if (*ptr == '-') ptr++; //can be a negative constant
    for(int i=0; i<strlen(ptr)-1; i++){
        if(!isdigit(ptr[i])) return 0;
    }
    return 1;
}

//checks if a value is valid (a cell or a constant)
int is_valid_value(char *value, int R, int C){
    if(value == NULL || *value == '\0') return 0;
    if(is_valid_cell(value, R, C)) return 1;
    if(is_valid_constant(value)) return 1;
    return 0;
}

//checks if a range is valid
int is_valid_range(char *range, int R, int C){
    if(range == NULL || *range == '\0') return 0;
    char *rptr;
    rptr = range;
    if(*range == '(' && *(range+strlen(range)-1) == ')'){
        rptr++;
        while(*rptr){
            if(*rptr == ':') break;
            rptr++;
        }
        if(!*rptr) return 0;
        //separate the two cells
        char cell1[100], cell2[100];
        strncpy(cell1, range+1, rptr-range-1);
        cell1[rptr-range-1] = '\0';
        strncpy(cell2, rptr+1, range+strlen(range)-2-rptr);
        cell2[range+strlen(range)-2-rptr] = '\0';
        //check if both cells are valid and the first cell is less than or equal to the second cell
        char* cello1 = cell1;
        char* cello2 = cell2;
        if(is_valid_cell(cell1, R, C) && is_valid_cell(cell2, R, C)){
            if(column_to_number(cello1) <= column_to_number(cello2)) return 1;
        }
    }
    return 0;
}

//check if the value in sleep is valid
int is_valid_sleep_value(char *value, int R, int C){
    if(value == NULL || *value == '\0') return 0;
    if(*value == '(' && *(value+strlen(value)-1) == ')'){
        //extract value from the brackets
        char val[100];
        strncpy(val, value+1, strlen(value)-2);
        val[strlen(value)-2] = '\0';
        return is_valid_value(val, R, C);
    }
    return 0;
}

//checks if a formula is valid
int is_valid_formula(char *input, int R, int C){
    //detect the equal sign
    char *eq;
    eq = input;
    while(*eq){
        if(*eq == '=') break;
        eq++;
    }
    if(!*eq) return 0;
    //separate LHS and RHS
    char lhs[100], rhs[100];
    char *rhs_ptr;
    rhs_ptr = rhs;
    strncpy(lhs, input, eq-input);
    lhs[eq-input] = '\0';
    strcpy(rhs, eq+1);
    //LHS should be a valid cell
    if(!is_valid_cell(lhs, R, C)) return 0;
    //formula is a function type that takes range
    if( strncmp(rhs_ptr, "MAX", strlen("MAX")) == 0 ||
    strncmp(rhs_ptr, "MIN", strlen("MIN")) == 0 ||
    strncmp(rhs_ptr, "AVG", strlen("AVG")) == 0 ||
    strncmp(rhs_ptr, "SUM", strlen("SUM")) == 0 ){
        return is_valid_range(rhs_ptr+3, R, C); 
    }
    if( strncmp(rhs_ptr, "STDEV", strlen("STDEV")) == 0){
        return is_valid_range(rhs_ptr+5, R, C);
    }
    //formula is a function type that takes a cell or a constant
    if( strncmp(rhs_ptr, "SLEEP", strlen("SLEEP")) == 0){
        return is_valid_sleep_value(rhs_ptr+5, R, C);
    }
    //formula is of type <VALUE> op <VALUE>
    //cells cannot be negative
    if(*rhs_ptr == '-'){
        if(!isdigit(*(rhs_ptr+1))) return 0;
        rhs_ptr++;
    }
    //detect the operator
    char *op;
    op = rhs_ptr;
    while(*op){
        if(*op == '+' || *op == '-' || *op == '*' || *op == '/') break;
        op++;
    }
    //no operations involved
    if(!*op){
        return is_valid_value(rhs_ptr, R, C);
    }
    //separate the two values, separate the two values
    char rhs1[100], rhs2[100];
    strncpy(rhs1, rhs_ptr, op - rhs_ptr);
    rhs1[op-rhs_ptr] = '\0';
    strcpy(rhs2, op+1);
    //check if both values are valid
    if(is_valid_value(rhs1, R, C) && is_valid_value(rhs2, R, C)) return 1;
    return 0;
}

//main validator function
int is_valid_input(char *inp, int R, int C){
    //remove all spaces and convert to uppercase for consistency
    remove_spaces(inp);
    to_uppercase(inp);
    //make a copy
    char input[100];
    strcpy(input, inp);
    //scroll and quit command type input
    if(strncmp(input, "W", strlen("W")) == 0){
        if(strlen(input) == strlen("W") ) return 1;
    }
    if(strncmp(input, "S", strlen("S")) == 0){
        if(strlen(input) == strlen("S")) return 1;
    }
    if(strncmp(input, "A", strlen("A")) == 0){
        if(strlen(input) == strlen("A")) return 1;
    }
    if(strncmp(input, "D", strlen("D")) == 0){
        if(strlen(input) == strlen("D")) return 1;
    }
    if(strncmp(input, "Q", strlen("Q")) == 0){
        if(strlen(input) == strlen("Q")) return 1;
    }
    //scroll to command type input
    if(strncmp(input, "SCROLL_TO", strlen("SCROLL_TO")) == 0){  
        return is_valid_cell(input+9, R, C); //should have a cell next
    }
    //output enable commands
    if(strncmp(input, "DISABLE_OUTPUT", strlen("DISABLE_OUTPUT")) == 0){
        if(strlen(input) == strlen("DISABLE_OUTPUT") ) return 1;
    }
    if(strncmp(input, "ENABLE_OUTPUT", strlen("ENABLE_OUTPUT")) == 0){
        if(strlen(input) == strlen("ENABLE_OUTPUT") ) return 1;
    }
    //formula type input
    if(is_valid_formula(input, R, C) == 1) {
        return 1;
    }
    //invalid input
    return 0;
}

//Parsing functions
// Assuming a valid input string

//removes the first character of a string
void remove_first_character(char *str){
    if(str == NULL) return;
    char *temp = str;
    while(*temp){
        *temp = *(temp+1);
        temp++;
    }
}

//inserts a character at the beginning of a string
void insert_character_beginning(char *str, char c){
    if(str == NULL) return;
    char *temp = str + strlen(str) - 1;
    while(*temp && temp > str){
        *temp = *(temp-1);
        temp --;
    }
    *temp = c;
}

//removes the second character of a substring in a string
void remove_second_of_substring(char *str, char *substr) {
    if (str == NULL || substr == NULL) return; // Handle NULL input
    char *temp = strstr(str, substr);
    if (temp && strlen(substr) >= 2) { 
        temp++;
        while (*temp) {
            *temp = *(temp + 1); 
            temp++;
        }
    }
}

//changes a character in a string to another character
void change_character(char *str, char old_char, char new_char) {
    if (str == NULL) return;
    while (*str) {
        if (*str == old_char) *str = new_char;
        str++;
    }
}

//function to count the number of operations in a formula
int num_ops(char *rhs){
    int count =0;
    while(*rhs){
        if(*rhs == '+' || *rhs == '-' || *rhs == '*' || *rhs == '/') count ++;
        rhs ++;
    }
    return count;
}

//there are two operators, one of them should be a '-' sign for a constant
int two_op(char *rhs){
    //first value negative : (-const) (+, -, *, /) (   )
    if(*rhs == '-'){
        //remove the negative sign
        remove_first_character(rhs);
        char *op_pos = strpbrk(rhs, "+-");
        if(op_pos && isdigit(*(op_pos+1))){
            //switch the operations + and - for const op const
            if(strstr(rhs, "+")) change_character(rhs, '+', '-');
            else if(strstr(rhs, "-")) change_character(rhs, '-', '+');
        }
        if (isdigit(*rhs)) return 1; //will have to mult -1 overall
    }
    //second value negative : (   ) (*, /) (-const)
    //operation *
    char *ptr1 = strstr(rhs, "*-");
    if(ptr1){
        remove_first_character(ptr1+1);
        if (isdigit(*(ptr1+1))) return 1; //will have to mult -1 overall
    }
    //operation /
    char *ptr2 = strstr(rhs, "/-");
    if(ptr2){
        remove_first_character(ptr2+1);
        if (isdigit(*(ptr2+1))) return 1; //will have to mult -1 overall
    }
    //operation + 
    char *ptr3 = strstr(rhs, "+-");
    if(ptr3){
        //simply +- becomes -
        remove_first_character(ptr3);
        return 0;
    }
    //operation -
    char *ptr4 = strstr(rhs, "--");
    if(ptr4){
        //simply -- becomes +
        remove_first_character(ptr4);
        remove_first_character(ptr4);
        insert_character_beginning(ptr4, '+');
        return 0;
    }
    return 0; 
} 

//three operators, two minus signs for two constants (should be distributed to two values)
int three_op(char *rhs){
    if(*rhs == '-'){
        //remove both negative signs
        remove_first_character(rhs);
        char *op_pos = strpbrk(rhs, "+-*/");
        if (*(op_pos+1) == '-'){
            remove_first_character(op_pos+1);
            if (*op_pos == '+' || *op_pos == '-') return 31; //will have to mult -1 overall
            else if (*op_pos == '*' || *op_pos == '/') return 32; //no need as two negatives cancel out
        }
    }
    return 0;
}

//from a cell name, extract the row number
int get_row_number(char *str) {
    while (*str && isalpha(*str)) str++;
    return atoi(str);
}

//fill the cell number and row number from a cell name
void parse_spreadsheet_coordinate(char *input, int *col_num, int *row_num) {
    *col_num = column_to_number(input);
    *row_num = get_row_number(input);
}

//separate the left hand side and right hand side of the formula
void separate_lhs_rhs(char *input, char *lhs, char *rhs) {
    char *optr = input;
    //left of the equal sign is LHS
    while (*optr && *optr != '=') {
        *lhs = *optr;
        lhs++;
        optr++;
    }
    *lhs = '\0';
    optr++;
    //rest is RHS
    while (*optr) {
        *rhs = *optr;
        rhs++;
        optr++;
    }
    *rhs = '\0';
}

//Identifying the operation involved (based on declared list of operations)
enum ops get_operation( char *rhs){
    //pointers to the operators, when called, should have only oine operator!
    char *plus_pos = strstr(rhs, "+");
    char *minus_pos = strstr(rhs, "-");
    char *mult_pos = strstr(rhs, "*");
    char *div_pos = strstr(rhs, "/");
    //check for function type operations
    if (strstr(rhs, "MAX")) return MAX;
    else if (strstr(rhs, "MIN")) return MIN;
    else if (strstr(rhs, "SUM")) return SUM;
    else if (strstr(rhs, "AVG")) return AVG;
    else if (strstr(rhs, "STDEV")) return STD_DEV;
    else if (strstr(rhs, "SLEEP")) return SLEEP_CONST;
    //check for <CELL> = <VALUE> op <VALUE> type operations
    else if (plus_pos){
        if(isalpha(*rhs)){
            rhs = plus_pos + 1;
            if (isalpha(*rhs)) return CELL_ADD_CELL;
            if (isdigit(*rhs)) return CELL_ADD_CONST;
        }
        rhs = plus_pos + 1; 
        if (isalpha(*rhs)) return CELL_ADD_CONST; //CONST_ADD_CELL actually 
        if (isdigit(*rhs)) return CONST; // CONST_ADD_CONST actually 
    }
    else if (mult_pos){
        if(isalpha(*rhs)){
            rhs = mult_pos + 1;
            if (isalpha(*rhs)) return CELL_MULT_CELL;
            if (isdigit(*rhs)) return CELL_MULT_CONST;    
        }
        rhs = mult_pos + 1;
        if (isalpha(*rhs)) return CELL_MULT_CONST; //CONST_MULT_CELL actually 
        if (isdigit(*rhs)) return CONST; //CONST_MULT_CONST actually 
    }
    else if (div_pos){
        if(isalpha(*rhs)){
            rhs = div_pos + 1;
            if (isalpha(*rhs)) return CELL_DIV_CELL;
            if (isdigit(*rhs)) return CELL_DIV_CONST;    
        }
        rhs = div_pos + 1;
        if (isalpha(*rhs)) return CONST_DIV_CELL;
        if (isdigit(*rhs)) return CONST; //CONST_DIV_CONST actually
    }
    else if (minus_pos){
        if(isalpha(*rhs)){
            rhs = minus_pos + 1;
            if (isalpha(*rhs)) return CELL_SUB_CELL;
            if (isdigit(*rhs)) return CELL_ADD_CONST; //CELL_SUB_CONST actually (const should be changed to negative)
        }
        rhs = minus_pos + 1;
        if (isalpha(*rhs)) return CONST_SUB_CELL;
        if (isdigit(*rhs)) return CONST; //CONST_SUB_CONST actually
    }
    //no operator, <CELL> = <VALUE> type operation
    else if (isalpha(*rhs)) return SINGLE_CELL;
    else if (isdigit(*rhs)) return CONST;
    else return 0;
}

//separator functions
//separate the two cells involved in the rhs into their coordinates
void separate_cells(char *rhs, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y) {
    char cell1[10], cell2[10];
    char *op = strpbrk(rhs, "+-*/");
    if (!op) return; // No operation found
    strncpy(cell1, rhs, op - rhs);
    cell1[op - rhs] = '\0';
    strcpy(cell2, op + 1);
    //fill coordinates
    parse_spreadsheet_coordinate(cell1, cell_1x, cell_1y);
    parse_spreadsheet_coordinate(cell2, cell_2x, cell_2y);
}

//separate the two cells involved in the range of a function
void separate_cells_range( char *rhs, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y) {
    char cell1[10], cell2[10];
    //of the form =SUM(B2:C2)
    char *lb = strpbrk(rhs, "("); 
    char *op = strpbrk(rhs, ":");
    char *rb = strpbrk(rhs, ")");
    if (!op) return; // No operation found
    strncpy(cell1, lb+1, op-lb-1);
    cell1[op-lb-1] = '\0';
    strncpy(cell2, op+1, rb-op-1);
    //fill coordinates
    parse_spreadsheet_coordinate(cell1, cell_1x, cell_1y);
    parse_spreadsheet_coordinate(cell2, cell_2x, cell_2y);
}

//extract the value from the SLEEP function
void separate_sleep (char *rhs, int *cell_1x, int *cell_1y, int *constant, int *flag){ 
    //of the form SLEEP(value) where value can be a cell or a constant
    char sleep_str[10];
    char *lb = strpbrk(rhs, "(");
    char *rb = strpbrk(rhs, ")");
    strncpy(sleep_str, lb+1, rb-lb-1);
    sleep_str[rb-lb-1] = '\0';
    if(isalpha(*sleep_str)){
        //value is a cell
        parse_spreadsheet_coordinate(sleep_str, cell_1x, cell_1y);
        *constant = 0;
        *flag = 1; //flag tells that the value is a cell
    }
    else{
        //value is a constant (can be negative as well)
        *constant = atoi(sleep_str);
        *cell_1x = 0;
        *cell_1y = 0;
    }
}

//separate the cell and the constant in the rhs
void separate_cell_constant(char *rhs, int *cell_1x, int *cell_1y, int *constant) {
    char cell[10];
    char constant_str[10];
    char *op = strpbrk(rhs, "+-*/");
    if (!op) return; // No operation found
    if(isalpha(*rhs)){
        //first value is a cell
        strncpy(cell, rhs, op - rhs);
        cell[op - rhs] = '\0';
        strcpy(constant_str, op + 1);
        *constant = atoi(constant_str);
        parse_spreadsheet_coordinate(cell, cell_1x, cell_1y);
    }
    else{
        //first value is a constant
        strncpy(constant_str, rhs, op - rhs);
        constant_str[op - rhs] = '\0';
        *constant = atoi(constant_str);
        strcpy(cell, op + 1);
        parse_spreadsheet_coordinate(cell, cell_1x, cell_1y);
    }   
}

//separate the two constants in the rhs (will be converted to one const later)
void separate_constant_constant(char *rhs, int *constant1, int *constant2) {
    char constant_str1[10], constant_str2[10];
    char *op = strpbrk(rhs, "+-*/");
    if (!op) return; // No operation found
    //fill the two constants (can be negative, but are not)
    strncpy(constant_str1, rhs, op - rhs);
    constant_str1[op - rhs] = '\0';
    *constant1 = atoi(constant_str1);
    strcpy(constant_str2, op + 1);
    *constant2 = atoi(constant_str2);
}

//Main parsing function
//not for dis_op, en_op and w,s,a,d,q, they are directly handled in main
void parse_input(char *input, int *constant, int *cell_ix, int *cell_iy, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y, int *operation){
    //at max 3 cells, 1 constant involved
    int flag_neg = 0; //flag to indicate mult by -1 overall
    char lhs[100], rhs[100];
    remove_spaces(input); //already done in the validator but here for independent usage
    //scroll to command, updates cell_i 
    if(strstr(input, "SCROLL_TO")){
        *operation = SCROLL;
        parse_spreadsheet_coordinate(input+9, cell_ix, cell_iy);
        *cell_1x = 0;
        *cell_1y = 0;
        *cell_2x = 0;
        *cell_2y = 0;
        *constant = 0;
        return;
    }
    //separate the LHS and RHS, formula inputs
    separate_lhs_rhs(input, lhs, rhs);
    //update cell_i from LHS
    parse_spreadsheet_coordinate(lhs, cell_ix, cell_iy);
    //no of operations in RHS
    int n = num_ops(rhs);
    if(n==2){if(two_op(rhs)) flag_neg = 1;} //mult by -1 const if two_op returns 1
    else if(n==3){if(three_op(rhs) == 31) flag_neg = 1;} //mult by -1 const if three_op returns 31
    //only one operator is involved in the string now
    //get the operation involved, update operation
    enum ops op = get_operation(rhs);
    *operation = op;
    //<CELL> = <CONST> handle two constants and resolve to one 
    if (op == CONST){
        char constant_str[10];
        char *op2 = strpbrk(rhs, "+-*/");
        if(op2){
            //two constants to one
            int constant1, constant2;
            separate_constant_constant(rhs, &constant1, &constant2);
            if(*op2 == '+') *constant = constant1 + constant2;
            else if(*op2 == '-') *constant = constant1 - constant2;
            else if(*op2 == '*') *constant = constant1 * constant2;
            else if(*op2 == '/') {
                if(constant2 == 0) {
                    //division by zero gives error state
                    *operation = ERR;
                    *constant = 0;
                }
                else *constant = constant1 / constant2;
            }
        }
        else{
            //single constant only
            strcpy(constant_str, rhs);
            *constant = atoi(constant_str);
        }
        //overall mult by -1
        if(flag_neg == 1){
            *constant = -(*constant);
            flag_neg = 0;
        }
        *cell_1x = 0;
        *cell_1y = 0;
        *cell_2x = 0;
        *cell_2y = 0;
        return;
    }
    //<CELL> = <CELL> update cell_1 from rhs
    else if(op == SINGLE_CELL){
        parse_spreadsheet_coordinate(rhs, cell_1x, cell_1y);
        *constant = 0;
        *cell_2x = 0;
        *cell_2y = 0;
        return;
    }
    //rhs involves CELL op CONST update cell_1 and constant
    else if(op == CELL_ADD_CONST || op == CELL_MULT_CONST || op == CELL_DIV_CONST || op == CONST_DIV_CELL || op == CONST_SUB_CELL){
        separate_cell_constant(rhs, cell_1x, cell_1y, constant);
        if(flag_neg == 1){
            //multiply const by -1 because flag raised
            *constant = -(*constant);
            flag_neg = 0;
        }
        *cell_2x = 0;
        *cell_2y = 0;
        return;
    }
    //rhs involves CELL op CELL update cell_1 and cell_2
    else if(op == CELL_ADD_CELL || op == CELL_SUB_CELL || op == CELL_MULT_CELL || op == CELL_DIV_CELL){
        separate_cells(rhs, cell_1x, cell_1y, cell_2x, cell_2y);
        *constant = 0;
        return;
    }
    //rhs invloves a function with range arguement update cell_1 and cell_2
    else if(op == MAX || op == MIN || op == SUM || op == AVG || op == STD_DEV){
        separate_cells_range(rhs, cell_1x, cell_1y, cell_2x, cell_2y);
        *constant = 0;
        return;
    }
    //rhs is sleep function update cell_1
    else if(op == SLEEP_CONST){
        int flag = 0;
        separate_sleep(rhs, cell_1x, cell_1y, constant, &flag);
        *cell_2x = 0;
        *cell_2y = 0;
        if(flag ==1){
            //flag tells argument to sleep is a cell
            *operation = SLEEP_CELL;
        }
        return;
    }
    return;
}

// //main function to test the parser and validator separately
// int main() {
//     while(10){
//         char input[100];
//         fgets(input, sizeof(input), stdin);
//         //input taken by fgets has a newline character \n at last
//         if (is_valid_input(input, 10, 10)) {
//             printf("YES\n");
//             int c, x1, y1, x2, y2, x3, y3, o;
//             parse_input(input, &c, &x1, &y1, &x2, &y2, &x3, &y3, &o);
//             printf("%s \n%d %d %d %d %d %d %d %d\n", input, x1, y1, x2, y2, x3, y3, c, o);
//         }
//         else{
//             printf("NO\n");
//         }
//     }
// }