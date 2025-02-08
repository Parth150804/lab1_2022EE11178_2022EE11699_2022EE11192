#include "../include/commands.h"

// to convert the numerical values in formula (stored as string) into integer
int string_to_int(const char *str) {
    if (str == NULL) {
        return 0; // Handle NULL input safely
    }

    int result = 0, sign = 1;
    
    // Skip leading whitespaces if any
    while (*str == ' ') {
        str++;
    }

    // Handle sign
    if (*str == '-' || *str == '+') {
        if (*str == '-') {
            sign = -1;
        }
        str++;
    }

    // Ignore leading zeros
    while (*str == '0') {
        str++;
    }

    // Convert characters to integer
    while (*str && isdigit(*str)) {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

// Function to convert column label to zero-based column index
int column_label_to_index(const char *col_label) {
    int col_index = 0;
    while (*col_label) {
        col_index = col_index * 26 + (*col_label - 'A' + 1);
        col_label++;
    }
    return col_index - 1;  // Convert to zero-based index
}

// Function to extract column and row from cell reference
void extract_column_row(const char *cell_ref, char *col_label, int *row_number) {
    int i = 0, j = 0;

    // Extract column label (letters)
    while (cell_ref[i] && isalpha(cell_ref[i])) {
        col_label[j++] = cell_ref[i++];
    }
    col_label[j] = '\0';  // Null terminate the column label

    // Extract row number (digits)
    *row_number = atoi(cell_ref + i)-1;
}

int performOpr(int x, int y, char op) {
    if (op == '+') {
        return x + y;
    } 
    else if (op == '-') {
        return x - y;
    } 
    else if (op == '*') {
        return x * y;
    } 
    else {
        return x / y;
    } 
}

// Function to check if the given range is valid and extract indices
int is_valid_range(const char *range, int *row1, int *col1, int *row2, int *col2) {
    char lower[10], upper[10]; // Buffers to store lower and upper cell references
    char col_label1[10], col_label2[10];

    // Find the ':' separator
    char *colon_ptr = strchr(range, ':');
    if (!colon_ptr) return 0; // No ':' found, invalid format

    // Split into two parts
    strncpy(lower, range, colon_ptr - range);
    lower[colon_ptr - range] = '\0'; // Null-terminate
    strcpy(upper, colon_ptr + 1);

    // Extract column and row indices for both bounds
    extract_column_row(lower, col_label1, row1);
    extract_column_row(upper, col_label2, row2);

    // Convert column labels to indices
    *col1 = column_label_to_index(col_label1);
    *col2 = column_label_to_index(col_label2);

    // Check for valid range (Lower bound should not exceed Upper bound)
    return (*row1 <= *row2) && (*col1 <= *col2);
}

// Find the minimum value in a given range
int min_range(Node **sheet, int row1, int col1, int row2, int col2) {
    int min_val = INT_MAX;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (sheet[i][j].value < min_val) {
                min_val = sheet[i][j].value;
            }
        }
    }
    return min_val;
}

// Find the maximum value in a given range
int max_range(Node **sheet, int row1, int col1, int row2, int col2) {
    int max_val = INT_MIN;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            if (sheet[i][j].value > max_val) {
                max_val = sheet[i][j].value;
            }
        }
    }
    return max_val;
}

// Compute the sum of values in a given range
int sum_range(Node **sheet, int row1, int col1, int row2, int col2) {
    int sum = 0;
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            sum += sheet[i][j].value;
        }
    }
    return sum;
}

// Compute the average of values in a given range
int avg_range(Node **sheet, int row1, int col1, int row2, int col2) {
    int sum = sum_range(sheet, row1, col1, row2, col2);
    int count = (row2 - row1 + 1) * (col2 - col1 + 1);
    return count > 0 ? sum / count : 0;  // Avoid division by zero
}

// Compute the standard deviation of values in a given range
int stdev_range(Node **sheet, int row1, int col1, int row2, int col2) {
    int sum = 0, count = 0;
    double mean, variance = 0.0;

    // Calculate sum and count
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            sum += sheet[i][j].value;
            count++;
        }
    }
    if (count == 0) return 0.0;  // Avoid division by zero

    mean = (double)sum / count;

    // Calculate variance
    for (int i = row1; i <= row2; i++) {
        for (int j = col1; j <= col2; j++) {
            variance += pow(sheet[i][j].value - mean, 2);
        }
    }

    variance /= count;  // Population standard deviation (use count - 1 for sample SD)
    return (int)sqrt(variance);
}

// Function to check if a given string is a valid cell reference
bool is_valid_cell_reference(const char *cell) {
    if (!cell || !(*cell)) return false; // Null or empty string

    int i = 0;
    
    // Check column part (must be uppercase letters A-Z)
    while (isupper(cell[i])) {
        i++;
    }

    if (i == 0 || i > 3) return false; // Column label should be between 1-3 letters (A-Z)

    // Check row part (must be digits)
    int rowStart = i;
    while (isdigit(cell[i])) {
        i++;
    }

    if (rowStart == i || i - rowStart > 3) return false; // Row must be at least 1 digit, at most 3

    return cell[i] == '\0'; // Ensure no extra characters
}

void extract_cell(const char *input, char *cell) {
    const char *start = strchr(input, ' '); // Find the first space
    if (start) {
        start++; // Move past the space
        while (*start && isspace((unsigned char)*start)) start++; // Skip extra spaces if any
        strcpy(cell, start); // Copy the cell part
    } else {
        cell[0] = '\0'; // If no space found, return an empty string
    }
}
