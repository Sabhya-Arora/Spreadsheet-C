#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


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
    SLEEP_CELL
};


// Assuming a valid input string

void remove_first_character(char *str){
    if(str == NULL) return;
    char *temp = str;
    while(*temp){
        *temp = *(temp+1);
        temp++;
    }
}


void remove_second_of_substring(char *str, const char *substr) {
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

// void rearrange_string(char *str, const char *substr) {
//     if (str == NULL || substr == NULL) return;

//     char *pos = strstr(str, substr);
//     if (!pos) return;

//     int index = pos - str;  // Position of the substring in `str`
//     int len_substr = strlen(substr);
//     int len = strlen(str);

//     char temp[100]; //stores rearranged string

//     // Copy the part after the substring to the beginning of `temp`
//     int temp_index = 0;
//     for (int i = index + len_substr; i < len; i++) {
//         if (str[i] != '\n') temp[temp_index++] = str[i];
//     }

//     // Copy the substring in place
//     for (int i = 0; i < len_substr; i++) {
//         temp[temp_index++] = substr[i];
//     }

//     // Copy the part before the substring to the end
//     for (int i = 0; i < index; i++) {
//         if(str[i] != '\n') temp[temp_index++] = str[i];
//     }

//     temp[temp_index] = '\0';
//     strcpy(str, temp);
// }

void change_character(char *str, char old_char, char new_char) {
    if (str == NULL) return;

    while (*str) {
        if (*str == old_char) *str = new_char;
        str++;
    }
}


//function to detect extra negative sign
int two_op(char *rhs){
    if(*rhs == '-'){
        remove_first_character(rhs);
        char *op_pos = strpbrk(rhs, "+-");
        if(op_pos && isdigit(*(op_pos+1))){
            if(strstr(rhs, "+")) change_character(rhs, '+', '-');
            else if(strstr(rhs, "-")) change_character(rhs, '-', '+');
        }
        if (isalpha(*rhs)) return 11; //negative cell first INVALID
        else if (isdigit(*rhs)) return 12; //negative constant first
    }
    char *ptr1 = strstr(rhs, "*-");
    if(ptr1){
        remove_second_of_substring(rhs, "*-");
        if (isalpha(*(ptr1+1))) return 21; //negative cell second INVALID
        else if (isdigit(*(ptr1+1))) return 22; //negative constant second
    }
    char *ptr2 = strstr(rhs, "/-");
    if(ptr2){
        remove_second_of_substring(rhs, "/-");
        if (isalpha(*(ptr2+1))) return 21; //negative cell second INVALID
        else if (isdigit(*(ptr2+1))) return 22; //negative constant second
    }
    return 0; 
} 

/*
two_op: 11 means (-cell) (+, -, *, /) (   )
two_op: 12 means (-const) (+, -, *, /) (   )
two_op: 21 means (   ) (*, /) (-cell)
two_op: 22 means (   ) (*, /) (-const)
*/


//converting cell index to coordinates
int column_to_number(const char *col) {
    int result = 0;
    while (*col) {
        if (!isalpha(*col)) break;
        result = result * 26 + (toupper(*col) - 'A' + 1);
        col++;
    }
    return result;
}

int get_row_number(const char *str) {
    while (*str && isalpha(*str)) str++;
    return atoi(str);
}

void parse_spreadsheet_coordinate(const char *input, int *col_num, int *row_num) {
    *col_num = column_to_number(input);
    *row_num = get_row_number(input);
}

//break into lhs and rhs
void remove_spaces(const char *input, char *output) {
    while (*input) {
        // printf("debug: %c\n", *input);
        if (!isspace(*input)) {
            *output = *input;
            output++;
        }
        input++;
    }
    *output = '\0';
}

void separate_lhs_rhs(const char *input, char *lhs, char *rhs) {
    char output[100];
    remove_spaces(input, output);
    char *optr = output;
    while (*optr && *optr != '=') {
        *lhs = *optr;
        lhs++;
        optr++;
    }
    *lhs = '\0';
    optr++;
    while (*optr) {
        *rhs = *optr;
        rhs++;
        optr++;
    }
    *rhs = '\0';
}


//Identifying the operation
enum ops get_operation(const char *rhs){
    // printf("debug: %s\n", rhs);
    char *plus_pos = strstr(rhs, "+");
    char *minus_pos = strstr(rhs, "-");
    char *mult_pos = strstr(rhs, "*");
    char *div_pos = strstr(rhs, "/");
    if (plus_pos){
        if(isalpha(*rhs)){
            rhs = plus_pos + 1;
            if (isalpha(*rhs)) return CELL_ADD_CELL;
            if (isdigit(*rhs)) return CELL_ADD_CONST;
        }
        rhs = plus_pos + 1; 
        if (isalpha(*rhs)) return CELL_ADD_CONST; //CONST_ADD_CELL actually (done)
        if (isdigit(*rhs)) return CONST; // CONST_ADD_CONST actually (done)
    }
    else if (mult_pos){
        if(isalpha(*rhs)){
            rhs = mult_pos + 1;
            if (isalpha(*rhs)) return CELL_MULT_CELL;
            if (isdigit(*rhs)) return CELL_MULT_CONST;    
        }
        rhs = mult_pos + 1;
        if (isalpha(*rhs)) return CELL_MULT_CONST; //CONST_MULT_CELL actually (done)
        if (isdigit(*rhs)) return CONST; //CONST_MULT_CONST actually (done)
    }
    else if (div_pos){
        if(isalpha(*rhs)){
            rhs = div_pos + 1;
            if (isalpha(*rhs)) return CELL_DIV_CELL;
            if (isdigit(*rhs)) return CELL_DIV_CONST;    
        }
        rhs = div_pos + 1;
        if (isalpha(*rhs)) return CONST_DIV_CELL;
        if (isdigit(*rhs)) return CONST; //CONST_DIV_CONST actually (done)
    }
    else if (minus_pos){
        if(isalpha(*rhs)){
            rhs = minus_pos + 1;
            if (isalpha(*rhs)) return CELL_SUB_CELL;
            if (isdigit(*rhs)) return CELL_ADD_CONST; //CELL_SUB_CONST actually (const change karna padega to negative) (done)
        }
        rhs = minus_pos + 1;
        if (isalpha(*rhs)) return CONST_SUB_CELL; //naya hai (done)
        if (isdigit(*rhs)) return CONST; //CONST_SUB_CONST actually (done)
    }
    else if (strstr(rhs, "MAX")) return MAX;
    else if (strstr(rhs, "MIN")) return MIN;
    else if (strstr(rhs, "SUM")) return SUM;
    else if (strstr(rhs, "AVG")) return AVG;
    else if (strstr(rhs, "STD_DEV")) return STD_DEV;
    else if (strstr(rhs, "SLEEP")) return SLEEP_CONST;

    else if (isalpha(*rhs)) return SINGLE_CELL;
    else if (isdigit(*rhs)) return CONST;
    else return 0;
}





//separator functions
void separate_cells(const char *rhs, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y) {
    char cell1[10], cell2[10];
    const char *op = strpbrk(rhs, "+-*/");
    if (!op) return; // No operation found

    strncpy(cell1, rhs, op - rhs);
    cell1[op - rhs] = '\0';
    strcpy(cell2, op + 1);

    parse_spreadsheet_coordinate(cell1, cell_1x, cell_1y);
    parse_spreadsheet_coordinate(cell2, cell_2x, cell_2y);
}

void separate_cells_range(const char *rhs, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y) {
    char cell1[10], cell2[10];
    const char *lb = strpbrk(rhs, "("); //of the form =SUM(B2:C2)
    const char *op = strpbrk(rhs, ":");
    const char *rb = strpbrk(rhs, ")");
    if (!op) return; // No operation found
    strncpy(cell1, lb+1, op-lb-1);
    cell1[op-lb-1] = '\0';
    strncpy(cell2, op+1, rb-op-1);

    parse_spreadsheet_coordinate(cell1, cell_1x, cell_1y);
    parse_spreadsheet_coordinate(cell2, cell_2x, cell_2y);
}

void separate_sleep (const char *rhs, int *cell_1x, int *cell_1y, int *constant, int *flag){ //of the form SLEEP(value) where value can be a cell or a constant
    char sleep_str[10];
    const char *lb = strpbrk(rhs, "(");
    const char *rb = strpbrk(rhs, ")");
    strncpy(sleep_str, lb+1, rb-lb-1);
    sleep_str[rb-lb-1] = '\0';
    if(isalpha(*sleep_str)){
        parse_spreadsheet_coordinate(sleep_str, cell_1x, cell_1y);
        *constant = 0;
        *flag = 1;
    }
    else{
        *constant = atoi(sleep_str);
        *cell_1x = 0;
        *cell_1y = 0;
    }
}

void separate_cell_constant(const char *rhs, int *cell_1x, int *cell_1y, int *constant) {
    char cell[10];
    char constant_str[10];
    const char *op = strpbrk(rhs, "+-*/");
    if (!op) return; // No operation found
    if(isalpha(*rhs)){
        char *minus_const_pos = strstr(rhs, "-");
        strncpy(cell, rhs, op - rhs);
        cell[op - rhs] = '\0';
        strcpy(constant_str, op + 1);
        *constant = atoi(constant_str);

        parse_spreadsheet_coordinate(cell, cell_1x, cell_1y);
    }
    else{
        strncpy(constant_str, rhs, op - rhs);
        constant_str[op - rhs] = '\0';
        *constant = atoi(constant_str);
        strcpy(cell, op + 1);

        parse_spreadsheet_coordinate(cell, cell_1x, cell_1y);
    }
    
}

void separate_constant_cell(const char *rhs, int *constant, int *cell_1x, int *cell_1y) {
    char cell[10];
    char constant_str[10];
    const char *op = strpbrk(rhs, "+-*/");
    if (!op) return; // No operation found

    strncpy(constant_str, rhs, op - rhs);
    constant_str[op - rhs] = '\0';
    *constant = atoi(constant_str);
    strcpy(cell, op + 1);

    parse_spreadsheet_coordinate(cell, cell_1x, cell_1y);
}

void separate_constant_constant(const char *rhs, int *constant1, int *constant2) {
    char constant_str1[10], constant_str2[10];
    const char *op = strpbrk(rhs, "+-*/");
    if (!op) return; // No operation found

    strncpy(constant_str1, rhs, op - rhs);
    constant_str1[op - rhs] = '\0';
    *constant1 = atoi(constant_str1);
    strcpy(constant_str2, op + 1);
    *constant2 = atoi(constant_str2);
}

//Main parse function  
int parse_input(const char *input, int *constant, int *cell_ix, int *cell_iy, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y, enum ops *operation){
    int flag_neg;
    char lhs[100], rhs[100];
    separate_lhs_rhs(input, lhs, rhs);
    parse_spreadsheet_coordinate(lhs, cell_ix, cell_iy);
    if(two_op(rhs)) flag_neg = 1;
    else flag_neg = 0;
    enum ops op = get_operation(rhs);
    *operation = op;
    if (op == CONST){
        char constant_str[10];
        char *op2 = strpbrk(rhs, "+-*/");
        if(op2){
            int constant1, constant2;
            separate_constant_constant(rhs, &constant1, &constant2);
            if(*op2 == '+') *constant = constant1 + constant2;
            else if(*op2 == '-') *constant = constant1 - constant2;
            else if(*op2 == '*') *constant = constant1 * constant2;
            else if(*op2 == '/') *constant = constant1 / constant2;
        }
        else{
            strcpy(constant_str, rhs);
            *constant = atoi(constant_str);
        }
        if(flag_neg == 1){
            *constant = -(*constant);
            flag_neg = 0;
        }
        *cell_1x = 0;
        *cell_1y = 0;
        *cell_2x = 0;
        *cell_2y = 0;
        return 1;
    }
    else if(op == SINGLE_CELL){
        parse_spreadsheet_coordinate(rhs, cell_1x, cell_1y);
        *constant = 0;
        *cell_2x = 0;
        *cell_2y = 0;
        return 1;
    }
    else if(op == CELL_ADD_CONST || op == CELL_MULT_CONST || op == CELL_DIV_CONST){
        separate_cell_constant(rhs, cell_1x, cell_1y, constant);
        if(op==CELL_ADD_CONST && strstr(rhs,"-")){
            *constant = -(*constant);
        }
        if(op == CELL_ADD_CONST && flag_neg == 1){
            *constant = -(*constant);
            flag_neg = 0;
        }
        if(op == CELL_MULT_CONST || op == CELL_DIV_CONST){
            if(flag_neg == 1){
                *constant = -(*constant);
                flag_neg = 0;
            }
        }
        *cell_2x = 0;
        *cell_2y = 0;
        return 1;
    }
    else if(op == CONST_DIV_CELL || op == CONST_SUB_CELL){
        separate_cell_constant(rhs, cell_1x, cell_1y, constant);
        if(op==CONST_DIV_CELL){
            if(flag_neg == 1){
                *constant = -(*constant);
                flag_neg = 0;
            }
        }
        if(op==CONST_SUB_CELL && flag_neg == 1){
            *constant = -(*constant);
            flag_neg = 0;
        }
        *cell_2x = 0;
        *cell_2y = 0;
        return 1;
    }
    else if(op == CELL_ADD_CELL || op == CELL_SUB_CELL || op == CELL_MULT_CELL || op == CELL_DIV_CELL){
        separate_cells(rhs, cell_1x, cell_1y, cell_2x, cell_2y);
        *constant = 0;
        return 1;
    }
    else if(op == MAX || op == MIN || op == SUM || op == AVG || op == STD_DEV){
        separate_cells_range(rhs, cell_1x, cell_1y, cell_2x, cell_2y);
        *constant = 0;
        return 1;
    }
    else if(op == SLEEP_CONST){
        int flag = 0;
        separate_sleep(rhs, cell_1x, cell_1y, constant, &flag);
        *cell_2x = 0;
        *cell_2y = 0;
        if(flag ==1){
            *operation = SLEEP_CELL;
        }
        return 1;
    }
    return 0;
}

int main() {
    int T =100;
    while(T--){
        char expression[100];
        fgets(expression, sizeof(expression), stdin);
        int constant, cell_ix, cell_iy, cell_1x, cell_1y, cell_2x, cell_2y;
        enum ops operation;
        // int flag_neg;
        parse_input(expression, &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation);
        printf("Constant: %d\n", constant);
        printf("Cell Index: %d, %d\n", cell_ix, cell_iy);
        printf("Cell 1: %d, %d\n", cell_1x, cell_1y);
        printf("Cell 2: %d, %d\n", cell_2x, cell_2y);
        printf("Operation: %d\n", operation);
        // printf("Flag: %d\n", flag_neg);
    }
    return 0;
}

//     CONST = 1,
//     SINGLE_CELL =2 ,
//     CELL_ADD_CONST = 3,
//     CELL_MULT_CONST = 4,
//     CELL_DIV_CONST =5,
//     CONST_DIV_CELL = 6,
//     CONST_SUB_CELL = 7,
//     CELL_ADD_CELL = 8,
//     CELL_SUB_CELL = 9,
//     CELL_MULT_CELL = 10,
//     CELL_DIV_CELL = 11,
//     MAX = 12,
//     MIN = 13,
//     SUM = 14,
//     AVG = 15,
//     STD_DEV = 16,
//     SLEEP = 17

 