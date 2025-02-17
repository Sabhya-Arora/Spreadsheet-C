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
        while(*d == ' '){
            ++d;
        }
    } while(*s++ = *d++);
}

int base26_digits(int x){
    if (x == 0) return 1; 
    return (int)(log10(x) / log10(26)) + 1;
}

int base10_digits(int x){
    if (x==0) return 1;
    return (int)(log10(x)) + 1;
}

int is_valid_cell(char *cell, int R, int C){
    int max_alpha = base26_digits(R);
    int max_num = base10_digits(C);
    int max_len = max_alpha + max_num + 1;
    if(cell == NULL || *cell == '\0') return 0;
    if(strlen(cell) > max_len) return 0;
    int alpha_count=0;
    int num_count=0;
    while(*cell && isalpha(*cell)){
        alpha_count++;
        cell++;
    }
    while(*cell && isdigit(*cell)){
        num_count++;
        cell++;
    }
    if(alpha_count==0 || alpha_count>max_alpha) return 0;
    if(num_count==0 || num_count>max_num) return 0;
    cell++;
    if(*cell == '\0') return 1;
    return 0;
}

int is_valid_constant(char *cst){
    if (cst == NULL || *cst == '\0') return 0;
    if (*cst == '-' || *cst == '+') cst++;
    for(int i=0; i<strlen(cst)-1; i++){
        if(!isdigit(cst[i])) return 0;
    }
    return 1;
}

int is_valid_value(char *value, int R, int C){
    if(is_valid_cell(value, R, C)) return 1;
    if(is_valid_constant(value)) {
        printf("AJD");
        return 1;
    }
    return 0;
}

int is_valid_range(char *range, int R, int C){
    if(range == NULL || *range == '\0') return 0;
    char *rptr;
    rptr = range;
    if(*range == '(' && *(range+strlen(range)-2) == ')'){
        rptr++;
        while(*rptr){
            if(*rptr == ':') break;
            rptr++;
        }
        if(!*rptr) return 0;

        char cell1[100], cell2[100];
        strncpy(cell1, range+1, rptr-range-1);
        cell1[rptr-range-1] = '\0';
        strncpy(cell2, rptr+1, range+strlen(range)-2-rptr-1);
        cell2[range+strlen(range)-2-rptr-1] = '\0';

        if(is_valid_cell(cell1, R, C) && is_valid_cell(cell2, R, C)){
            if(strcmp(cell1, cell2) <= 0) return 1;
        }

    }
    return 0;
}

int is_valid_sleep_value(char *value, int R, int C){
    if(value == NULL || *value == '\0') return 0;
    if(*value == '(' && *(value+strlen(value)-2) == ')'){
        char val[100];
        strncpy(val, value+1, strlen(value)-3);
        val[strlen(value)-3] = '\0';
        if(*val == '-') return 0;
        return is_valid_value(val, R, C);
    }
    return 0;
}

int is_valid_formula(char *input, int R, int C){
    char *eq;
    eq = input;
    while(*eq){
        if(*eq == '=') break;
        eq++;
    }
    if(!*eq) return 0;

    char lhs[100], rhs[100];
    char *rhs_ptr;
    rhs_ptr = rhs;
    strncpy(lhs, input, eq-input);
    lhs[eq-input] = '\0';
    strcpy(rhs, eq+1);

    if(!is_valid_cell(lhs, R, C)) return 0;
    if( strncmp(rhs_ptr, "MAX", strlen("MAX")) == 0 ||
    strncmp(rhs_ptr, "MIN", strlen("MIN")) == 0 ||
    strncmp(rhs_ptr, "AVG", strlen("AVG")) == 0 ||
    strncmp(rhs_ptr, "SUM", strlen("SUM")) == 0 ){
        return is_valid_range(rhs_ptr+3, R, C);
    }
    if( strncmp(rhs_ptr, "STDEV", strlen("STDEV")) == 0){
        return is_valid_range(rhs_ptr+5, R, C);
    }
    if( strncmp(rhs_ptr, "SLEEP", strlen("SLEEP")) == 0){
        return is_valid_sleep_value(rhs_ptr+5, R, C);
    }
    
    if(*rhs_ptr == '-' || *rhs_ptr == '+'){
        if(!isdigit(*(rhs_ptr+1))) return 0;
        rhs_ptr++;
    }
    
    char *op;
    op = rhs_ptr;
    while(*op){
        if(*op == '+' || *op == '-' || *op == '*' || *op == '/') break;
        op++;
    }
    if(!*op){
        printf("SB");
        return is_valid_value(rhs_ptr, R, C);
    }
    
    char rhs1[100], rhs2[100];
    strncpy(rhs1, rhs_ptr, op - rhs_ptr);
    rhs1[op-rhs_ptr] = '\0';
    strcpy(rhs2, op+1);

    if(is_valid_value(rhs, R, C) && is_valid_value(rhs2, R, C)) return 1;

    return 0;
}

int is_valid_input(const char *inp, int R, int C){
    remove_spaces(inp);
    to_uppercase(inp);
    char input[100];
    strcpy(input, inp);
    
    if(strncmp(input, "W", strlen("W")) == 0){
        if(strlen(input) == strlen("W") + 1) return 1;
    }
    if(strncmp(input, "S", strlen("S")) == 0){
        if(strlen(input) == strlen("S") + 1) return 1;
    }
    if(strncmp(input, "A", strlen("A")) == 0){
        if(strlen(input) == strlen("A") + 1) return 1;
    }
    if(strncmp(input, "D", strlen("D")) == 0){
        if(strlen(input) == strlen("D") + 1) return 1;
    }
    if(strncmp(input, "Q", strlen("Q")) == 0){
        if(strlen(input) == strlen("Q") + 1) return 1;
    }
    
    if(strncmp(input, "SCROLL_TO", strlen("SCROLL_TO")) == 0){  
        return is_valid_cell(input+9, R, C);
    }
    if(strncmp(input, "DISABLE_OUTPUT", strlen("DISABLE_OUTPUT")) == 0){
        if(strlen(input) == strlen("DISABLE_OUTPUT") + 1) return 1;
    }
    if(strncmp(input, "ENBLE_OUTPUT", strlen("ENABLE_OUTPUT")) == 0){
        if(strlen(input) == strlen("ENABLE_OUTPUT") + 1) return 1;
    }
    // printf("HD");
    if(is_valid_formula(input, R, C) == 1) return 1;
    
    return 0;
}















// Assuming a valid input string

void remove_first_character(char *str){
    if(str == NULL) return;
    char *temp = str;
    while(*temp){
        *temp = *(temp+1);
        temp++;
    }
}

void insert_character_beginning(char *str, const char c){
    if(str == NULL) return;
    char *temp = str + strlen(str) - 1;
    while(*temp && temp > str){
        *temp = *(temp-1);
        temp --;
    }
    *temp = c;
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

void change_character(char *str, char old_char, char new_char) {
    if (str == NULL) return;

    while (*str) {
        if (*str == old_char) *str = new_char;
        str++;
    }
}

int num_ops(const char *rhs){
    int count =0;
    while(*rhs){
        if(*rhs == '+' || *rhs == '-' || *rhs == '*' || *rhs == '/') count ++;
        rhs ++;
    }
    return count;
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
        if (isalpha(*rhs)) return 11; //negative cell first
        else if (isdigit(*rhs)) return 12; //negative constant first
    }
    char *ptr1 = strstr(rhs, "*-");
    if(ptr1){
        remove_first_character(ptr1+1);
        if (isalpha(*(ptr1+1))) return 21; //negative cell second
        else if (isdigit(*(ptr1+1))) return 22; //negative constant second
    }
    char *ptr2 = strstr(rhs, "/-");
    if(ptr2){
        remove_first_character(ptr2+1);
        if (isalpha(*(ptr2+1))) return 21; //negative cell second
        else if (isdigit(*(ptr2+1))) return 22; //negative constant second
    }
    char *ptr3 = strstr(rhs, "+-");
    if(ptr3){
        remove_first_character(ptr3);
        return 0;
    }
    char *ptr4 = strstr(rhs, "--");
    if(ptr4){
        remove_first_character(ptr4);
        remove_first_character(ptr4);
        insert_character_beginning(ptr4, '+');
        printf("%s\n", rhs);
        return 0;
    }
    return 0; 
} 

int three_op(char *rhs){
    if(*rhs == '-'){
        remove_first_character(rhs);
        char *op_pos = strpbrk(rhs, "+-*/");
        if (*(op_pos+1) == '-'){
            remove_first_character(op_pos+1);
            // printf("%s\n", rhs);
            // printf("%c\n", *op_pos);
            if (*op_pos == '+' || *op_pos == '-') return 31;
            else if (*op_pos == '*' || *op_pos == '/') return 32;
        }
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
// void remove_spaces(char* s) {
//     char* d = s;
//     do {
//         while (*d == ' ') {
//             ++d;
//         }
//     } while (*s++ = *d++);
// }

void separate_lhs_rhs(char *input, char *lhs, char *rhs) {
    char *optr = input;
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
int parse_input(char *input, int *constant, int *cell_ix, int *cell_iy, int *cell_1x, int *cell_1y, int *cell_2x, int *cell_2y, int *operation){
    int flag_neg = 0;
    char lhs[100], rhs[100];
    remove_spaces(input);
    if(strstr(input, "scroll_to")){
        *operation = SCROLL;
        printf("%s\n", input);
        parse_spreadsheet_coordinate(input+9, cell_ix, cell_iy);
        *cell_1x = 0;
        *cell_1y = 0;
        *cell_2x = 0;
        *cell_2y = 0;
        *constant = 0;
        return 1;
         
    }
    separate_lhs_rhs(input, lhs, rhs);
    parse_spreadsheet_coordinate(lhs, cell_ix, cell_iy);
    int n = num_ops(rhs);
    if(n==2){if(two_op(rhs)) flag_neg = 1;}
    else if(n==3){if(three_op(rhs) == 31) flag_neg = 1;}
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
            else if(*op2 == '/') {
                if(constant2 == 0) {
                    *operation = ERR;
                    *constant = 0;
                }
                else *constant = constant1 / constant2;
            }
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

// int main() {
//     int T =100;
//     while(T--){
//         char expression[100];
//         fgets(expression, sizeof(expression), stdin);
//         int constant, cell_ix, cell_iy, cell_1x, cell_1y, cell_2x, cell_2y;
//         enum ops operation;
//         // int flag_neg;
//         parse_input(expression, &constant, &cell_ix, &cell_iy, &cell_1x, &cell_1y, &cell_2x, &cell_2y, &operation);
//         printf("Constant: %d\n", constant);
//         printf("Cell Index: %d, %d\n", cell_ix, cell_iy);
//         printf("Cell 1: %d, %d\n", cell_1x, cell_1y);
//         printf("Cell 2: %d, %d\n", cell_2x, cell_2y);
//         printf("Operation: %d\n", operation);
//     }



 