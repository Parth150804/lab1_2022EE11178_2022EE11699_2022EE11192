#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define a structure for each cell (node in the graph)
typedef struct Node {
    int value;                // Integer value of the cell
    char *expression;         // Expression (if any) stored as a string
    struct Node **dependencies; // Array of pointers to dependent nodes
    int dep_count;            // Number of dependencies
} Node;

// Function to create a new node
Node *create_node() {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->value = 0;
    new_node->expression = NULL;
    new_node->dependencies = NULL;
    new_node->dep_count = 0;
    return new_node;
}

// Function to initialize the sheet as a 1D array of nodes
void initialize_sheet(Node **sheet, int nrows, int ncols) {
    for (int i = 0; i < nrows * ncols; i++) {
        sheet[i] = create_node();
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
void print_column_headers(int ncols) {
    int max_cols = ncols > 10 ? 10 : ncols;
    for (int j = 0; j < max_cols; j++) {
        char label[10];
        get_column_label(j, label);
        printf("%-6s", label);
    }
    printf("\n");
}

// Function to print sheet data
void print_sheet_data(Node **sheet, int nrows, int ncols) {
    int max_rows = nrows > 10 ? 10 : nrows;
    int max_cols = ncols > 10 ? 10 : ncols;
    for (int i = 0; i < max_rows; i++) {
        printf("%-3d   ", i + 1);
        for (int j = 0; j < max_cols; j++) {
            printf("%-6d", sheet[i * ncols + j]->value);
        }
        printf("\n");
    }
}

// Function to display the sheet
void display_sheet(Node **sheet, int nrows, int ncols) {
    printf("      ");
    print_column_headers(ncols);
    print_sheet_data(sheet, nrows, ncols);
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

    initialize_sheet(sheet, nrows, ncols);
    display_sheet(sheet, nrows, ncols);

    for (int i = 0; i < nrows * ncols; i++) {
        free(sheet[i]);
    }
    free(sheet);

    return 0;
}

