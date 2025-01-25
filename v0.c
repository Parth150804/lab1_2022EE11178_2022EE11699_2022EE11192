#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function to convert column index to Excel-style label (A, B, ..., Z, AA, AB, ..., ZZZ)
void get_column_label(int col_index, char *label) {
  int i = 0;
  while (col_index >= 0) {
    label[i++] = 'A' + (col_index % 26);
    col_index = col_index / 26 - 1;
  }
  label[i] = '\0';

  // Reverse the label string because the calculation gives it in reverse order
  for (int j = 0, k = i - 1; j < k; j++, k--) {
    char temp = label[j];
    label[j] = label[k];
    label[k] = temp;
  }
}

// Function to print the column headers (Excel-like A, B, C, ..., AA, AB, ..., ZZZ)
void print_column_headers(int ncols) {
  int max_cols = ncols > 10 ? 10 : ncols;
  for (int j = 0; j < max_cols; j++) {
    char label[10];
    get_column_label(j, label);
    printf("%-6s", label);  // Adjust width to 6 for proper spacing
  }
  printf("\n");
}

// Function to initialize the sheet with example data
void initialize_sheet(int *sheet, int nrows, int ncols) {
  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < ncols; j++) {
      *(sheet + i * ncols + j) = 0;  // Example data: unique integer for each cell
    }
  }
}

// Function to print row headers and data of the sheet
void print_sheet_data(const int *sheet, int nrows, int ncols) {
  int max_rows = nrows > 10 ? 10 : nrows;
  int max_cols = ncols > 10 ? 10 : ncols;
  for (int i = 0; i < max_rows; i++) {
    printf("%-3d   ", i + 1);  // Row header, adjusted width to 3 for proper spacing
    for (int j = 0; j < max_cols; j++) {
      printf("%-6d", *(sheet + i * ncols + j));  // Sheet data, adjusted width to 6
    }
    printf("\n");
  }
}

// Function to display the entire sheet (with headers and data)
void display_sheet(const int *sheet, int nrows, int ncols) {
  printf("      ");  // Padding for the top-left corner
  print_column_headers(ncols);  // Print the column headers
  print_sheet_data(sheet, nrows, ncols);  // Print the sheet data with row headers
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <nrows> <ncols>\n", argv[0]);
    return 1;
  }

  // Convert command-line arguments to integers
  int nrows = atoi(argv[1]);
  int ncols = atoi(argv[2]);

  // Validate the range of rows and columns
  if (nrows < 1 || nrows > 999 || ncols < 1 || ncols > 18278) {
    printf("Invalid range. Rows must be between 1 and 999, and columns between 1 and 18278.\n");
    return 1;
  }

  // Dynamically allocate the sheet as a 1D array (for flexibility)
  int *sheet = (int *)malloc(nrows * ncols * sizeof(int));
  if (!sheet) {
    printf("Memory allocation failed.\n");
    return 1;
  }

  // Initialize the sheet with example data
  initialize_sheet(sheet, nrows, ncols);

  // Display the sheet
  display_sheet(sheet, nrows, ncols);

  // Free allocated memory
  free(sheet);

  return 0;
}