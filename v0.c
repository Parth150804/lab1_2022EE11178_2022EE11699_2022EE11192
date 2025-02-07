#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include "./spreadsheet-v1/src/commands.c"
#include "./spreadsheet-v1/src/input_handling.c"

// Function to create a new node
Node *create_node() {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed for node.\n");
        return NULL; // Return NULL to indicate failure
    }
    new_node->value = 0;
    new_node->expression = NULL;
    new_node->dependencies = NULL;
    new_node->dep_count = 0;
    return new_node;
}

// Function to initialize the sheet as a 2D array of nodes
void initialize_sheet(Node **sheet, int nrows, int ncols) {
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            sheet[i][j] = *create_node();  // Assuming create_node() returns a pointer, dereference it
        }
    }
}

// Function to get the column label (A, B, ..., Z, AA, AB, ..., ZZZ)
void get_column_label(int col_index, char *label) {
    int i = 0;
    while (col_index >= 0) {
        label[i++] = 'A' + (col_index % 26);
        col_index = col_index / 26 - 1;
    }
    label[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = label[j];
        label[j] = label[k];
        label[k] = temp;
    }
}

// Function to print column headers
void print_column_headers(int start_col, int ncols) {
    int max_cols = ncols-start_col > 10 ? 10 + start_col : ncols;
    for (int j = start_col; j < max_cols; j++) {
        char label[10];
        get_column_label(j, label);
        printf("%-6s", label);
    }
    printf("\n");
}

// Function to print sheet data starting from a specified row and column
void print_sheet_data(Node **sheet, int nrows, int ncols, int start_row, int start_col) {
    // Ensure start indices are within bounds
    if (start_row >= nrows || start_col >= ncols) {
        start_col = 0;
        start_row = 0;
    }
    // Limit printing to a max of 10 rows and 10 columns
    int max_rows = (start_row + 10 > nrows) ? (nrows - start_row) : 10;
    int max_cols = (start_col + 10 > ncols) ? (ncols - start_col) : 10;

    // Print data with row numbers
    for (int i = start_row; i < start_row + max_rows; i++) {
        printf("%-6d", i + 1);  // Row index
        for (int j = start_col; j < start_col + max_cols; j++) {
            printf("%-6d", sheet[i][j].value);  // Corrected access
        }
        printf("\n");
    }
}

// Function to display the sheet
void display_sheet(Node **sheet, int nrows, int ncols, int start_row, int start_col) {
    printf("      ");
    print_column_headers(start_col, ncols);
    print_sheet_data(sheet, nrows, ncols, start_row, start_col);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <nrows> <ncols>\n", argv[0]);
        return 1;
    }

    int nrows = atoi(argv[1]);
    int ncols = atoi(argv[2]);

    if (nrows < 1 || nrows > 999 || ncols < 1 || ncols > 18278) {
        printf("Invalid range. Rows must be between 1 and 999, and columns between 1 and 18278.\n");
        return 1;
    }

    Node **sheet = (Node **)malloc(nrows * ncols * sizeof(Node *));
    if (!sheet) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Allocate memory for each row
    for (int i = 0; i < nrows; i++) {
        sheet[i] = (Node *)malloc(ncols * sizeof(Node));
        if (!sheet[i]) {
            printf("Memory allocation failed.\n");
            return 1;
        }
    }

    initialize_sheet(sheet, nrows, ncols);

    bool print_output = true;

        time_t start_time = time(NULL);  // Start measuring time

        if (print_output) display_sheet(sheet, nrows, ncols, 0, 0);

        time_t end_time = time(NULL);  // End measuring time
        double elapsed_time = (double)(end_time - start_time);

        int start_row = 0;
        int start_col = 0;
        char status[100];
        strcpy(status, "ok");

    while (true) {
        printf("[%.1f] (%s) > ", elapsed_time, status);  // Print execution time and prompt

        char input[256];
        char cell[16];
        Expression expr;

        fgets(input, sizeof(input), stdin);  // Read the entire input line including spaces
        input[strcspn(input, "\n")] = 0;     // Remove the newline character from fgets 

        bool valid = parse_input(input, cell, &expr);
        if (valid) {
            if (strcmp(expr.type, "control") == 0) {
                int r, c;
                strcpy(status, "ok");
                if (strcmp(expr.value[0], "w") == 0) {
                    r = fmax(start_row-10, 0);
                    c = start_col;
                }
                else if (strcmp(expr.value[0], "d") == 0) {
                    r = start_row;
                    c = fmin(start_col+10, ncols);
                }
                else if (strcmp(expr.value[0], "a") == 0) {
                    r = start_row;
                    c = fmax(start_col-10, 0);
                }
                else if (strcmp(expr.value[0], "s") == 0) {
                    r = fmin(start_row+10, nrows);
                    c = start_col;
                }
                else if (strcmp(expr.value[0], "q") == 0) {
                    return 0;
                }
                else {
                    strcpy(status, "unrecognized cmd");
                }
                start_time = time(NULL);  
                if (r != nrows) start_row = r;
                if (c != ncols) start_col = c;

                display_sheet(sheet, nrows, ncols, start_row, start_col);

                end_time = time(NULL);  
                elapsed_time = (double)(end_time - start_time);
            } 
            else if (strcmp(expr.type, "constant") == 0) {
                int val = string_to_int(expr.value[0]);
                char col_label[100];
                int r, c;
                extract_column_row(cell, col_label, &r);
                c = column_label_to_index(col_label);

                start_time = time(NULL); 
                if (r >= nrows || c >= ncols) {
                    strcpy(status, "Invalid range");
                }
                else {
                    sheet[r][c].value = val;
                    strcpy(status, "ok");
                } 

                display_sheet(sheet, nrows, ncols, start_row, start_col);

                end_time = time(NULL);  
                elapsed_time = (double)(end_time - start_time);
            } 
            else if (strcmp(expr.type, "arithmetic") == 0) {
                char col_label[100];
                int r, c;
                extract_column_row(cell, col_label, &r);
                c = column_label_to_index(col_label);

                start_time = time(NULL); 
                if (r >= nrows || c >= ncols) {
                    strcpy(status, "Invalid range");
                }
                else {
                    strcpy(status, "ok");
                    int x, y;
                    if (is_valid_cell_reference(expr.value[0])) {
                        char col_label[100];
                        int r, c;
                        extract_column_row(expr.value[0], col_label, &r);
                        c = column_label_to_index(col_label);
                        x = sheet[r][c].value; 
                    }
                    else {
                        x = string_to_int(expr.value[0]);
                    }
                    if (is_valid_cell_reference(expr.value[1])) {
                        char col_label[100];
                        int r, c;
                        extract_column_row(expr.value[1], col_label, &r);
                        c = column_label_to_index(col_label);
                        y = sheet[r][c].value; 
                    }
                    else {
                        y = string_to_int(expr.value[1]);
                    }
                    char op = expr.operator[0];
                    if (op == '/' && y == 0) { 
                        strcpy(sheet[r][c].expression, "ERR");
                    }
                    else {
                        sheet[r][c].value = performOpr(x, y, op);
                    }
                }
                display_sheet(sheet, nrows, ncols, start_row, start_col);

                end_time = time(NULL);  
                elapsed_time = (double)(end_time - start_time); 
            } 
            else {
                start_time = time(NULL); 
                
                char col_label[100];
                int r, c;
                extract_column_row(cell, col_label, &r);
                c = column_label_to_index(col_label);

                if (r >= nrows || c >= ncols) {
                    strcpy(status, "Invalid range");
                }
                else if (strcmp(expr.function, "SLEEP") == 0) {
                    char col_label1[100];
                    int r1, c1;
                    extract_column_row(expr.range, col_label1, &r1);
                    c1 = column_label_to_index(col_label1);

                    if (r >= nrows || c >= ncols) {
                        strcpy(status, "Invalid range");
                    }
                    else {
                        strcpy(status, "ok");
                        int val = sheet[r1][c1].value;
                        sleep(val);
                        sheet[r][c].value = val;
                    }
                    display_sheet(sheet, nrows, ncols, start_row, start_col);
        
                    end_time = time(NULL);  
                    elapsed_time = (double)(end_time - start_time);
                }
                else {
                    strcpy(status, "ok");
                    int row1, col1, row2, col2;
                    if (is_valid_range(expr.range, &row1, &col1, &row2, &col2)) {
                        if (strcmp(expr.function, "MIN") == 0) {
                            sheet[r][c].value = min_range(sheet, row1, col1, row2, col2);
                        } 
                        else if (strcmp(expr.function, "MAX") == 0) {
                            sheet[r][c].value = max_range(sheet, row1, col1, row2, col2);
                        } 
                        else if (strcmp(expr.function, "AVG") == 0) {
                            sheet[r][c].value = avg_range(sheet, row1, col1, row2, col2);
                        } 
                        else if (strcmp(expr.function, "SUM") == 0) {
                            sheet[r][c].value = sum_range(sheet, row1, col1, row2, col2);
                        } 
                        else if (strcmp(expr.function, "STDEV") == 0) {
                            sheet[r][c].value = stdev_range(sheet, row1, col1, row2, col2);
                        }
                        else {
                            strcpy(status, "unrecognized cmd");
                        }
                        display_sheet(sheet, nrows, ncols, start_row, start_col);
        
                        end_time = time(NULL);  
                        elapsed_time = (double)(end_time - start_time); 
                    }
                    else {
                        strcpy(status, "Invalid range");
                    }
                }
            }
        }
        else {
            strcpy(status, "unrecognized cmd");
        }
    }

    for (int i = 0; i < nrows; i++) {
        free(sheet[i]);
    }
    free(sheet);

    return 0;
}

