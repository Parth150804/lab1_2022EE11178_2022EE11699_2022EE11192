#include <stdio.h>
#include <List.c>

typedef struct Cell {
    int value;                // Integer value of the cell
    char *formula;            // Formula as a string (if any)
    List *dependencies;       // List of cells this cell depends on
    List *dependents;         // List of cells that depend on this cell
} Cell;

typedef struct Spreadsheet {
    int rows;                 // Number of rows in the grid
    int cols;                 // Number of columns in the grid
    Cell **grid;              // Pointer to a 2D array of cells
} Spreadsheet;

typedef struct Range {
    int startRow, startCol;   // Starting indices of the range
    int endRow, endCol;       // Ending indices of the range
} Range;

// To access cells like A1, B1, etc., use a utility function that maps column names to indices:
// A → 0, Z → 25, AA → 26, ..., ZZZ → 18277.

int getColumnIndex(const char *colName) {
    int index = 0;
    for (int i = 0; colName[i] != '\0'; i++) {
        index = index * 26 + (colName[i] - 'A' + 1);
    }
    return index - 1; // 0-based index
}

// Example initialization
Spreadsheet *createSpreadsheet(int rows, int cols) {
    Spreadsheet *sheet = malloc(sizeof(Spreadsheet));
    sheet->rows = rows;
    sheet->cols = cols;
    sheet->grid = malloc(rows * sizeof(Cell *));
    for (int i = 0; i < rows; i++) {
        sheet->grid[i] = calloc(cols, sizeof(Cell)); // Initialize each row
    }
    return sheet;
}
