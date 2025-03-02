#include "LinkedList.h"
#include "Decl.h"

void calc (struct Cell * current, struct Cell ** spreadsheet);
void recalc (struct Cell * current, struct Cell * par_cell, int old_value,bool was_faulty, struct Cell ** spreadsheet);
void add_child(struct Cell* current, struct Cell ** spreadsheet);
bool cycle_detect(struct Cell* current, struct Cell** spreadsheet, int rows, int cols, struct Cell * tobepar1, struct Cell* tobepar2, int operation);
void toposort (struct Cell * par_cell, struct Cell * curr_cell, struct LinkedListNode ** ptr_to_head, bool ** vis);
void assign_new_attributes(struct Cell* current, struct Cell ** spreadsheet, struct Cell*parent1, struct Cell*parent2, int constant, int operation);
void remove_child(struct Cell* current, struct Cell ** spreadsheet);
void free_topos (struct LinkedListNode * head);


void delay(int number_of_seconds) {
    if (number_of_seconds < 0) number_of_seconds = 0;
    struct timespec ts;
    ts.tv_sec = number_of_seconds;  // Whole seconds
    ts.tv_nsec = 0;                 // Nanoseconds (optional)
    nanosleep(&ts, NULL);
}



bool update_cell(struct Cell* current, struct Cell ** spreadsheet, struct Cell*parent1, struct Cell*parent2, int constant, int operation, int totalrow, int totalcol) {
    int old_val = current -> value;
    bool was_faulty = current->is_faulty;
    // detect cycle first
    bool is_cycle_fr = cycle_detect(current, spreadsheet, totalrow, totalcol, parent1, parent2, operation);
    if (is_cycle_fr) return false;
    // remove this child from old parents
    remove_child(current, spreadsheet);
    // update current cell value and assign local attributes
    assign_new_attributes(current, spreadsheet, parent1, parent2, constant, operation);
    // add this cell to its parents
    add_child(current, spreadsheet);
    // toposort
    struct LinkedListNode* topos = NULL;
    bool** visited = (bool **)malloc(totalrow*sizeof(bool *));
    for(int i = 0; i<totalrow; i++){
        visited[i] = (bool *) malloc(totalcol*sizeof(bool));
    }
    for (int i = 0; i < totalrow; i++) {
        for (int j = 0; j < totalcol; j++) {
            visited[i][j] = false;
        }
    }
    toposort(current, current, &topos, visited);
    // call recalc for all children of this cell now
    for(int i = 0; i< totalrow; i++){
        free(visited[i]);
    }
    free(visited);
    struct LinkedListNode* cur = topos;
    while (cur != NULL) {
        cur = cur -> next;
    }
    cur = topos;
    cur = cur -> next;
    while (cur != NULL) {
        if ((cur -> value) != NULL) {
            struct Cell temp = *(cur->value);
        }
        recalc(cur -> value, cur -> par_cell, old_val,was_faulty, spreadsheet);
        cur = cur -> next;
    }
    free_topos(topos);
    return true;
}
void free_topos (struct LinkedListNode * head){
    if(head==NULL) return;
    free_topos(head->next);
    free(head);
}
void remove_child(struct Cell* current, struct Cell ** spreadsheet) {
    if (current -> num_parents <= 2) {
        if (current->par1 != NULL) {
            remove_cell(&(current -> par1 -> children), current);
        }
        if (current -> par2 != NULL) {
            remove_cell(&(current -> par2 -> children), current);
        }
    } else {
        int startrow, startcol, endrow, endcol;
        startrow = current -> par1 -> row;
        startcol = current -> par1 -> col;
        endrow = current -> par2 -> row;
        endcol = current -> par2 -> col;
        for (int i = startrow; i <= endrow; i++) {
            for (int j = startcol; j <= endcol; j++) {
                remove_cell(&(spreadsheet[i][j].children), current);
            }
        }
    }
}

void assign_new_attributes(struct Cell* current, struct Cell ** spreadsheet, struct Cell*parent1, struct Cell*parent2, int constant, int operation) {
    current -> operation = operation;
    current-> is_faulty = false;
    if (operation==ERR) current-> is_faulty = true;
    if (operation == CONST) {
        current -> num_parents = 0;
        current -> value = constant;
    } else if (operation == SINGLE_CELL) {
        current -> num_parents = 1;
        current -> par1 = parent1;
    } else if (operation >= 3 && operation <= 7) {
        current -> num_parents = 1;
        current -> par1 = parent1;
        current -> associated_const = constant;
    } else if (operation >= 8 && operation <= 11) {
        current -> num_parents = 2;
        current -> par1 = parent1;
        current -> par2 = parent2;
    } else if (operation == MAX || operation == MIN || operation == SUM) {
        current -> par1 = parent1;
        current -> par2 = parent2;
        int startrow, startcol, endrow, endcol;
        startrow = current -> par1 -> row;
        startcol = current -> par1 -> col;
        endrow = current -> par2 -> row;
        endcol = current -> par2 -> col;
        current -> num_parents = (endrow - startrow + 1) * (endcol - startcol + 1);
    } else if (operation == AVG || operation == STD_DEV) {
        current -> par1 = parent1;
        current -> par2 = parent2;
        int startrow, startcol, endrow, endcol;
        startrow = current -> par1 -> row;
        startcol = current -> par1 -> col;
        endrow = current -> par2 -> row;
        endcol = current -> par2 -> col;
        current -> num_parents = (endrow - startrow + 1) * (endcol - startcol + 1);
    } else if (operation == SLEEP_CONST) {
        current -> num_parents = 0;
        current -> associated_const = constant;
    } else if (operation == SLEEP_CELL) {
        current -> num_parents = 1;
        current -> par1 = parent1;
    }
    calc(current, spreadsheet);
    
}

void add_child(struct Cell* current, struct Cell ** spreadsheet) {
    if (current -> num_parents == 0) return;
    if (current -> num_parents == 1) {
        if (current->par1 != NULL) {
            add_cell(&(current -> par1 -> children), current);
        }
    } else if (current -> num_parents == 2) {
        if (current -> par1 != NULL) {
            add_cell(&(current -> par1 -> children), current);
        }
        if (current -> par2 != NULL) {
            add_cell(&(current -> par2 -> children), current);
        }
    } else {
        int startrow, startcol, endrow, endcol;
        startrow = current -> par1 -> row;
        startcol = current -> par1 -> col;
        endrow = current -> par2 -> row;
        endcol = current -> par2 -> col;
        for (int i = startrow; i <= endrow; i++) {
            for (int j = startcol; j <= endcol; j++) {
                add_cell(&(spreadsheet[i][j].children), current);
            }
        }
    }
}

bool dfs(struct Cell* current, bool ** wanna_be_pars, bool ** visited) {
    bool ans = false;
    visited[current -> row][current -> col] = true;
    for (struct Node * cur = current->children; cur != NULL; cur = cur -> next) {
        if (!visited[cur -> key -> row][cur -> key -> col]) {
            ans = ans || dfs(cur -> key, wanna_be_pars, visited);
        } else if (wanna_be_pars[cur -> key -> row][cur -> key -> col]) {
            return true;
        }
    }
    return ans;
}

bool cycle_detect(struct Cell* current, struct Cell** spreadsheet, int rows, int cols, struct Cell * tobepar1, struct Cell* tobepar2, int operation) {
    bool** wanna_be_pars = (bool **) malloc(rows*sizeof(bool *)),** visited = (bool **) malloc(rows*sizeof(bool *));
    for(int i = 0; i<rows; i++){
        wanna_be_pars[i] = (bool *) malloc(cols*sizeof(bool));
        visited[i] = (bool *) malloc(cols*sizeof(bool));
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            visited[i][j] = false;
            wanna_be_pars[i][j]=false;
        }
    }

    if ((operation >= 2 && operation <= 7) || operation == SLEEP_CELL) {
        wanna_be_pars[tobepar1 -> row][tobepar1 -> col] = true;
        visited[tobepar1 -> row][tobepar1 -> col] = true;
    } else if (operation >= 8 && operation <= 11) {
        wanna_be_pars[tobepar1 -> row][tobepar1 -> col] = true;
        visited[tobepar1 -> row][tobepar1 -> col] = true;
        wanna_be_pars[tobepar2 -> row][tobepar2 -> col] = true;
        visited[tobepar2 -> row][tobepar2 -> col] = true;
    } else if (operation >= 12 && operation <= 16) {
        int startrow, startcol, endrow, endcol;
        startrow = tobepar1 -> row;
        startcol = tobepar1 -> col;
        endrow = tobepar2 -> row;
        endcol = tobepar2 -> col;
        for (int i = startrow; i <= endrow; i++) {
            for (int j = startcol; j <= endcol; j++) {
                wanna_be_pars[i][j] = true;
                visited[i][j] = true;
            }
        }
    }

    bool cycle_exists = dfs(current, wanna_be_pars, visited);
    if (wanna_be_pars[current -> row][current -> col]) cycle_exists = true;
    for (int i = 0; i < rows; i++) {
        free(wanna_be_pars[i]);
        free(visited[i]);
    }
    free(wanna_be_pars);
    free(visited);
    return cycle_exists;
}

void recalc (struct Cell * current, struct Cell * par_cell, int old_value,bool was_faulty, struct Cell ** spreadsheet){
    current->is_faulty = false;
    switch (current->operation){
        case SINGLE_CELL:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = current->par1->value;
            break;
        case CELL_ADD_CONST:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) + (current->associated_const);
            break;
        case CELL_MULT_CONST:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value)*(current->associated_const);
            break;
        case CELL_DIV_CONST:
            if(current->par1->is_faulty || current->associated_const==0) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value)/(current->associated_const);
            break;
        case CONST_DIV_CELL:
            if(current->par1->is_faulty || current->par1->value==0) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->associated_const)/(current->par1->value);
            break;
        case CONST_SUB_CELL:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->associated_const)-(current->par1->value);
            break;
        case CELL_ADD_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) + (current->par2->value);
            break;
        case CELL_SUB_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) - (current->par2->value);
            break;
        case CELL_MULT_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) * (current->par2->value);
            break;
        case CELL_DIV_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty || current->par2->value==0) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) / (current->par2->value);
            break;
        case MAX :{
            int maxx = - __INT32_MAX__;
            int start_row = current->par1->row;
            int start_col = current->par1->col;
            int end_row = current->par2->row;
            int end_col = current->par2->col;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    if(spreadsheet[i][j].is_faulty) {
                        current->is_faulty = true;
                        return;
                    }
                    maxx = max(maxx, spreadsheet[i][j].value);
                }
            }
            current->value = maxx;
            break;}
        case MIN :{
            int minn =  __INT32_MAX__;
            int start_row = current->par1->row;
            int start_col = current->par1->col;
            int end_row = current->par2->row;
            int end_col = current->par2->col;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    if(spreadsheet[i][j].is_faulty) {
                        current->is_faulty = true;
                        return;
                    }
                    minn = min(minn, spreadsheet[i][j].value);
                }
            }
            current->value = minn;
            break;}
        case SUM :
            if(was_faulty) {
                calc(current, spreadsheet);
                return;
            }
            current->value = current->value + par_cell->value - old_value;
            break;
        case AVG :
            if(was_faulty) {
                calc(current, spreadsheet);
                return;
            }
            current->associated_sum = current->associated_sum + par_cell->value - old_value;
            current->value  = (current->associated_sum)/(current->num_parents);
            break;
        case STD_DEV :
            calc(current, spreadsheet);
            break;
        case SLEEP_CELL:{
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            int delaytime = current -> par1 -> value;
            delay(delaytime);
            current -> value = delaytime;
            break;}
        default:
            break;
    }

}

void toposort (struct Cell * par_cell, struct Cell * curr_cell, struct LinkedListNode ** ptr_to_head, bool ** vis){
    if (vis[curr_cell->row][curr_cell->col]) return;
    vis[curr_cell->row][curr_cell->col] = true;
    for (struct Node* ptr = curr_cell->children; ptr != NULL; ptr = ptr -> next) {
        toposort(curr_cell, ptr -> key, ptr_to_head, vis);
    }
    // printf("In toposort %d %d\n", curr_cell -> row, curr_cell -> col);
    struct LinkedListNode *newhead =  (struct LinkedListNode *) malloc (sizeof(struct LinkedListNode));
    newhead->par_cell = par_cell;
    newhead->value = curr_cell;
    newhead->next = *ptr_to_head;
    *ptr_to_head = newhead;
}
void calc (struct Cell * current, struct Cell ** spreadsheet){
    switch (current->operation){
        case SINGLE_CELL:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = current->par1->value;
            break;
        case CELL_ADD_CONST:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) + (current->associated_const);
            break;
        case CELL_MULT_CONST:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value)*(current->associated_const);
            break;
        case CELL_DIV_CONST:
            if(current->par1->is_faulty || current->associated_const==0) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value)/(current->associated_const);
            break;
        case CONST_DIV_CELL:
            if(current->par1->is_faulty || current->par1->value==0) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->associated_const)/(current->par1->value);
            break;
        case CONST_SUB_CELL:
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->associated_const)-(current->par1->value);
            break;
        case CELL_ADD_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) + (current->par2->value);
            break;
        case CELL_SUB_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) - (current->par2->value);
            break;
        case CELL_MULT_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) * (current->par2->value);
            break;
        case CELL_DIV_CELL:
            if(current->par1->is_faulty || current->par2->is_faulty || current->par2->value==0) {
                current->is_faulty = true;
                return;
            }
            current->value = (current->par1->value) / (current->par2->value);
            break;
        case MAX :{
            int maxx = - __INT32_MAX__;
            int start_row = current->par1->row;
            int start_col = current->par1->col;
            int end_row = current->par2->row;
            int end_col = current->par2->col;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    if(spreadsheet[i][j].is_faulty) {
                        current->is_faulty = true;
                        return;
                    }
                    maxx = max(maxx, spreadsheet[i][j].value);
                }
            }
            current->value = maxx;
            break;}
        case MIN :{
            int minn =  __INT32_MAX__;
            int start_row = current->par1->row;
            int start_col = current->par1->col;
            int end_row = current->par2->row;
            int end_col = current->par2->col;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    if(spreadsheet[i][j].is_faulty) {
                        current->is_faulty = true;
                        return;
                    }
                    minn = min(minn, spreadsheet[i][j].value);
                }
            }
            current->value = minn;
            break;}
        case SUM :{
            int sum =  0;
            int start_row = current->par1->row;
            int start_col = current->par1->col;
            int end_row = current->par2->row;
            int end_col = current->par2->col;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    if(spreadsheet[i][j].is_faulty) {
                        current->is_faulty = true;
                        return;
                    }
                    sum+=spreadsheet[i][j].value;
                }
            }
            current->value = sum;
            break;}
        case AVG :{
            int sum =  0;
            int start_row = current->par1->row;
            int start_col = current->par1->col;
            int end_row = current->par2->row;
            int end_col = current->par2->col;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    if(spreadsheet[i][j].is_faulty) {
                        current->is_faulty = true;
                        return;
                    }
                    sum+=spreadsheet[i][j].value;
                }
            }
            current -> associated_sum = sum;
            sum/=(current->num_parents);
            current->value = sum;
            break;}
        case STD_DEV :{
            int sum =  0;
            int start_row = current->par1->row;
            int start_col = current->par1->col;
            int end_row = current->par2->row;
            int end_col = current->par2->col;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    if(spreadsheet[i][j].is_faulty) {
                        current->is_faulty = true;
                        return;
                    }
                    sum+=spreadsheet[i][j].value;
                }
            }
            current -> associated_sum = sum;
            int mean = sum/(current->num_parents);
            double var = 0.0;
            for(int i = start_row; i<=end_row; i++){
                for(int j = start_col; j<=end_col;j++){
                    var+=((spreadsheet[i][j].value - mean)*(spreadsheet[i][j].value - mean));
                }
            }
            var/=(current->num_parents);
            int std_dev = (int)round(sqrt(var));
            current->value = std_dev;
            break;}
        case SLEEP_CONST:{
            int delaytime = current -> associated_const;
            delay(delaytime);
            current -> value = delaytime;
            break;}
        case SLEEP_CELL:{
            if(current->par1->is_faulty) {
                current->is_faulty = true;
                return;
            }
            int delaytime = current -> par1 -> value;
            delay(delaytime);
            current -> value = delaytime;
            break;}
        default:
            break;
    }

}