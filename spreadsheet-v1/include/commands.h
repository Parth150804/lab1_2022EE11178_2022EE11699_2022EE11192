#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// Define a structure for each cell (node in the graph)
typedef struct Node {
    int value;                // Integer value of the cell
    char *expression;         // Expression (if any) stored as a string
    struct Node **dependencies; // Array of pointers to dependent nodes
    int dep_count;            // Number of dependencies
    bool error;
} Node;

int string_to_int(const char *str);

int column_label_to_index(const char *col_label);

void extract_column_row(const char *cell_ref, char *col_label, int *row_number);

int performOpr(int x, int y, char op);

int is_valid_range(const char *range, int *row1, int *col1, int *row2, int *col2);

int min_range(Node **sheet, int row1, int col1, int row2, int col2);

int max_range(Node **sheet, int row1, int col1, int row2, int col2);

int sum_range(Node **sheet, int row1, int col1, int row2, int col2);

int avg_range(Node **sheet, int row1, int col1, int row2, int col2);

int stdev_range(Node **sheet, int row1, int col1, int row2, int col2);

bool is_valid_cell_reference(const char *cell);


