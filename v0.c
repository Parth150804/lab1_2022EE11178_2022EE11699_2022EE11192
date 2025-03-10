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
Node *create_node()
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node)
    {
        printf("Memory allocation failed for node.\n");
        return NULL; // Return NULL to indicate failure
    }
    new_node->value = 0;
    new_node->expression = (char *)malloc(strlen("ERR") + 1);
    new_node->dependencies = NULL;
    new_node->dependents = NULL;
    new_node->depc_count = 0;
    new_node->dept_count = 0;
    new_node->error = false;
    new_node->cellexpr = (Expression *)malloc(sizeof(Expression));

    if (!new_node->cellexpr)
    {
        printf("Memory allocation failed for Expression.\n");
        free(new_node);
        return NULL;
    }

    return new_node;
}

// Function to initialize the sheet as a 2D array of nodes
void initialize_sheet(Node **sheet, int nrows, int ncols)
{
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            sheet[i][j] = *create_node(); // Assuming create_node() returns a pointer, dereference it
        }
    }
}

// Function to get the column label (A, B, ..., Z, AA, AB, ..., ZZZ)
void get_column_label(int col_index, char *label)
{
    int i = 0;
    while (col_index >= 0)
    {
        label[i++] = 'A' + (col_index % 26);
        col_index = col_index / 26 - 1;
    }
    label[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--)
    {
        char temp = label[j];
        label[j] = label[k];
        label[k] = temp;
    }
}

// Function to print column headers
void print_column_headers(int start_col, int ncols)
{
    int max_cols = ncols - start_col > 10 ? 10 + start_col : ncols;
    for (int j = start_col; j < max_cols; j++)
    {
        char label[10];
        get_column_label(j, label);
        printf("%-4s", label);
    }
    printf("\n");
}

// Function to print sheet data starting from a specified row and column
void print_sheet_data(Node **sheet, int nrows, int ncols, int start_row, int start_col)
{
    // Ensure start indices are within bounds
    if (start_row >= nrows || start_col >= ncols)
    {
        start_col = 0;
        start_row = 0;
    }
    // Limit printing to a max of 10 rows and 10 columns
    int max_rows = (start_row + 10 > nrows) ? (nrows - start_row) : 10;
    int max_cols = (start_col + 10 > ncols) ? (ncols - start_col) : 10;

    // Print data with row numbers
    for (int i = start_row; i < start_row + max_rows; i++)
    {
        printf("%-4d", i + 1); // Row index
        for (int j = start_col; j < start_col + max_cols; j++)
        {
            if (!sheet[i][j].error)
            {
                printf("%-4d", sheet[i][j].value); // Corrected access
            }
            else
            {
                printf("%-4s", sheet[i][j].expression);
            }
        }
        printf("\n");
    }
}

// Function to display the sheet
void display_sheet(Node **sheet, int nrows, int ncols, int start_row, int start_col, bool flag)
{
    if (flag == false)
    {
        return;
    }
    printf("    ");
    print_column_headers(start_col, ncols);
    print_sheet_data(sheet, nrows, ncols, start_row, start_col);
}

void print_expression(const struct Expression *expr)
{
    if (!expr)
    {
        printf("Invalid Expression (NULL pointer).\n");
        return;
    }

    printf("Expression Details:\n");
    printf("--------------------\n");

    // Print the type of the expression
    printf("Type: %s\n", expr->type);

    // Print the values (if applicable)
    if (strlen(expr->value[0]) > 0 || strlen(expr->value[1]) > 0)
    {
        printf("Values: [%s], [%s]\n", expr->value[0], expr->value[1]);
    }
    else
    {
        printf("Values: None\n");
    }

    // Print the operator (if applicable)
    if (strlen(expr->operator) > 0)
    {
        printf("Operator: %s\n", expr->operator);
    }
    else
    {
        printf("Operator: None\n");
    }

    // Print the function (if applicable)
    if (strlen(expr->function) > 0)
    {
        printf("Function: %s\n", expr->function);
    }
    else
    {
        printf("Function: None\n");
    }

    // Print the range (if applicable)
    if (strlen(expr->range) > 0)
    {
        printf("Range: %s\n", expr->range);
    }
    else
    {
        printf("Range: None\n");
    }

    printf("--------------------\n");
}

// Remove 'dependent' from the dependency cell's dependents list.
//
void remove_dependent(Node *dependency, Node *dependent)
{
    if (!dependency || !dependency->dependents || !dependent)
    {
        // printf("NULL return\n");
        //;
        return;
    }
    int index = -1;
    for (int i = 0; i < dependency->dept_count; i++)
    {
        if (dependency->dependents[i] == dependent)
        {
            index = i;
            break;
        }
    }
    if (index != -1)
    {
        // Shift elements left.
        for (int i = index; i < dependency->dept_count - 1; i++)
        {
            dependency->dependents[i] = dependency->dependents[i + 1];
        }
        dependency->dept_count--;
        if (dependency->dept_count == 0)
        {
            free(dependency->dependents);
            dependency->dependents = NULL;
        }
        else
        {
            dependency->dependents = realloc(dependency->dependents,
                                             dependency->dept_count * sizeof(Node *));
        }
        // printf("Removed {%d} from {%d}.dependent\n", dependency->value, dependent->value);
        //;
    }
    // printf("Can't  {%d} from {%d}.dependent\n", dependency->value, dependent->value);
    //;
}

// Clear all dependency links from a cell.
// For each dependency in cell->dependencies, remove cell from that dependency’s dependents list.
void clear_dependencies(Node *cell)
{
    if (cell->dependencies != NULL)
    {
        for (int i = 0; i < cell->depc_count; i++)
        {
            // printf("cell to removed %d\n", cell->dependencies[i]->value);
            remove_dependent(cell->dependencies[i], cell);
        }
        free(cell->dependencies);
        cell->dependencies = NULL;
        cell->depc_count = 0;
        //;
    }
}

// Function to add a dependency between two nodes
void add_dependency(Node *dependent, Node *dependency)
{
    if (!dependent || !dependency)
    {
        printf("Invalid nodes provided.\n");
        return;
    }
    // Reallocate memory to add one more dependency
    Node **temp = NULL;
    if (dependent->depc_count == 0)
    {
        temp = (Node **)malloc(sizeof(Node *));
    }
    else
    {
        temp = realloc(dependent->dependencies, (dependent->depc_count + 1) * sizeof(Node *));
    }
    if (!temp)
    {
        printf("Memory allocation failed while adding dependency.\n");
        return;
    }
    // Reassign the new memory address after realloc
    dependent->dependencies = temp;

    // Add the new dependency
    dependent->dependencies[dependent->depc_count] = dependency;
    dependent->depc_count++; // Increment depc_count

    // Debug print statements to check the changes
    // printf("Inside add_dependency: depc_count = %d\n", dependent->depc_count);
}

void add_dependents(Node *dependent, Node *dependency)
{
    // Reallocate memory to add one more dependent
    Node **temp = NULL;
    if (dependency->dept_count == 0)
    {
        temp = (Node **)malloc(sizeof(Node *));
    }
    else
        temp = realloc(dependency->dependents, (dependency->dept_count + 1) * sizeof(Node *));
    if (!temp)
    {
        printf("Memory allocation failed while adding dependents.\n");
        return;
    }
    // Reassign the new memory address after realloc
    dependency->dependents = temp;

    // Add the new dependent
    dependency->dependents[dependency->dept_count] = dependent;
    dependency->dept_count++; // Increment dept_count
    // printf("Inside add_dependendent: dept_count = %d\n", dependency->dept_count);
}

// Recursive function to recalculate the cell
void recalculate_cell(Node **sheet, int nrows, int ncols, Node *node)
{
    if (!node)
        return; // Skip if node is NULL

    // For arithmetic expressions:
    if (node->cellexpr && strcmp(node->cellexpr->type, "arithmetic") == 0)
    {
        int x = 0, y = 0;
        char op = node->cellexpr->operator[0];
        bool dependencyError = false;

        // Evaluate first operand
        if (is_valid_cell_reference(node->cellexpr->value[0]))
        {
            char col_label[10];
            int row, col;
            extract_column_row(node->cellexpr->value[0], col_label, &row);
            col = column_label_to_index(col_label);
            if (sheet[row][col].error)
                dependencyError = true;
            x = sheet[row][col].value;
        }
        else
        {
            x = string_to_int(node->cellexpr->value[0]);
        }

        // Evaluate second operand
        if (is_valid_cell_reference(node->cellexpr->value[1]))
        {
            char col_label[10];
            int row, col;
            extract_column_row(node->cellexpr->value[1], col_label, &row);
            col = column_label_to_index(col_label);
            if (sheet[row][col].error)
                dependencyError = true;
            y = sheet[row][col].value;
        }
        else
        {
            y = string_to_int(node->cellexpr->value[1]);
        }

        // If any dependency is in error or division by zero occurs, mark error
        if (dependencyError || (op == '/' && y == 0))
        {
            node->error = true;
            strcpy(node->expression, "ERR");
        }
        else
        {
            node->error = false;
            node->value = performOpr(x, y, op);
        }
    }
    // For function expressions:
    else if (node->cellexpr && strcmp(node->cellexpr->type, "function") == 0)
    {
        int row1, col1, row2, col2;
        bool dependencyError = false;
        if (strcmp(node->cellexpr->function, "SLEEP") == 0)
        {
            int sleep_seconds = 0;
            bool dependencyError = false;

            // Parse the argument (cell or literal)
            if (is_valid_cell_reference(node->cellexpr->range))
            {
                char col_label[10];
                int row, col;
                extract_column_row(node->cellexpr->range, col_label, &row);
                col = column_label_to_index(col_label);

                // Check bounds and errors
                if (row >= nrows || col >= ncols || sheet[row][col].error)    
                {
                    dependencyError = true;
                }
                else
                {
                    // Recalculate dependency first
                    // recalculate_cell(sheet, nrows, ncols, &sheet[row][col]);
                    sleep_seconds = sheet[row][col].value;
                }
            }
            else
            {
                // Parse literal value
                // print_expression(node->cellexpr);
                sleep_seconds = string_to_int(node->cellexpr->range);
            }

            if (dependencyError)
            {
                printf("Dependency error in SLEEP function.\n");
                node->error = true;
                strcpy(node->expression, "ERR");
            }
            else
            {
                // printf("ERR SLEEP function.\n");
                sleep(sleep_seconds); // Actual sleep
                node->value = sleep_seconds;
                node->error = false;
            }
        }
        else if (is_valid_range(node->cellexpr->range, &row1, &col1, &row2, &col2))
        {
            // Check every cell in the range for an error
            for (int i = row1; i <= row2 && !dependencyError; i++)
            {
                for (int j = col1; j <= col2; j++)
                {
                    if (sheet[i][j].error)
                    {
                        dependencyError = true;
                        break;
                    }
                }
            }
            if (dependencyError)
            {
                node->error = true;
                strcpy(node->expression, "ERR");
            }
            else
            {
                // Evaluate the function normally
                if (strcmp(node->cellexpr->function, "MIN") == 0)
                {
                    node->value = min_range(sheet, row1, col1, row2, col2);
                }
                else if (strcmp(node->cellexpr->function, "MAX") == 0)
                {
                    node->value = max_range(sheet, row1, col1, row2, col2);
                }
                else if (strcmp(node->cellexpr->function, "SUM") == 0)
                {
                    node->value = sum_range(sheet, row1, col1, row2, col2);
                }
                else if (strcmp(node->cellexpr->function, "AVG") == 0)
                {
                    node->value = avg_range(sheet, row1, col1, row2, col2);
                }
                else if (strcmp(node->cellexpr->function, "STDEV") == 0)
                {
                    node->value = stdev_range(sheet, row1, col1, row2, col2);
                }
                node->error = false;
            }
        }
        else
        {
            node->error = true;
            strcpy(node->expression, "ERR");
        }
    }

    // Propagate changes to dependents (even if this node is in error)
    for (int i = 0; i < node->dept_count; i++)
    {
        if (node->dependents[i] == NULL)
        {
            // printf("NULL\n");
            continue;
        }
        // printf("ERR: recalculate_cell: %d\n", node->dependents[i]->value);
        if (node->dependents[i]->error && node->error)
        {
            continue;
        }
        recalculate_cell(sheet, nrows, ncols, node->dependents[i]);
    }
}

// Helper function to detect cycles
bool detect_cycle(Node *current, Node *target, Node **stack, int stackSize)
{
    if (current == NULL || target == NULL)
    {
        return false;
    }

    if (current == target)
        return true; // Cycle detected

    // Check if current is already in the recursion stack
    // printf("stackSize: %d : %d\n", stackSize, current->value);
    for (int i = 0; i < stackSize; i++)
    {
        // printf("stack[%d]: %d\n", i, stack[i]->value);
        if (stack[i] == current)
            return true; // Already visited in this path
    }
    // printf("helooooo from detect cycle\n");
    // Add current to the recursion stack
    stack[stackSize] = current;
    stackSize++;

    // Recursively check all dependencies
    // printf("depc_count in the : %d :- val:%d\n", current->depc_count, current->value);
    for (int i = 0; i < current->depc_count; i++)
    {
        if (current->dependencies == NULL)
        {
            // printf("NULL_List\n");
            return false;
        }
        if (current->dependencies[i] == NULL)
        {
            // printf("NULL\n");
            return false;
        }
        Node *dep = current->dependencies[i];
        // printf("went from %d -> %d\n", current->value, dep->value);

        if (dep == target)
        {
            // printf("True exit when curr_val= %d\n", current->value);
            return true; // Cycle detected
        }
        if (detect_cycle(dep, target, stack, stackSize))
            return true;
    }
    // printf("False exit when curr_val= %d\n", current->value);
    return false; // No cycle detected
}

int main(int argc, char *argv[])
{

    int nrows;
    int ncols;
    if (argc != 3)
    {
        nrows=999;
        ncols=18278;
    }
    else{
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
    }

    if (nrows < 1 || nrows > 999 || ncols < 1 || ncols > 18278)
    {
        printf("Invalid range.\n");
        return 1;
    }

    time_t start_time = time(NULL); // Start measuring time

    Node **sheet = (Node **)malloc(nrows * sizeof(Node *));
    if (!sheet)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Allocate memory for each row
    for (int i = 0; i < nrows; i++)
    {
        sheet[i] = (Node *)malloc(ncols * sizeof(Node));
        if (!sheet[i])
        {
            printf("Memory allocation failed.\n");
            return 1;
        }
    }

    initialize_sheet(sheet, nrows, ncols);

    bool print_output = true;

    if (print_output)
        display_sheet(sheet, nrows, ncols, 0, 0, print_output);

    time_t end_time = time(NULL); // End measuring time
    double elapsed_time = (double)(end_time - start_time);

    int start_row = 0;
    int start_col = 0;
    char status[100];
    strcpy(status, "ok");

    while (true)
    {
        printf("[%.1f] (%s) > ", elapsed_time, status); // Print execution time and prompt

        char input[256];
        char cell[16];
        Expression expr;

        fgets(input, sizeof(input), stdin); // Read the entire input line including spaces
        input[strcspn(input, "\n")] = 0;    // Remove the newline character from fgets

        if (strcmp(input, "disable_output") == 0)
        {
            print_output = false;
        }

        if (strcmp(input, "enable_output") == 0)
        {
            print_output = true;
        }

        bool valid = parse_input(input, cell, &expr);
        // print_expression(&expr);

        if (valid)
        {
            if (strcmp(expr.type, "control") == 0)
            {
                int r, c;
                strcpy(status, "ok");
                if (strcmp(expr.value[0], "w") == 0)
                {
                    r = fmax(start_row - 10, 0);
                    c = start_col;
                }
                else if (strcmp(expr.value[0], "d") == 0)
                {
                    r = start_row;
                    c = fmin(start_col + 10, ncols);
                }
                else if (strcmp(expr.value[0], "a") == 0)
                {
                    r = start_row;
                    c = fmax(start_col - 10, 0);
                }
                else if (strcmp(expr.value[0], "s") == 0)
                {
                    r = fmin(start_row + 10, nrows);
                    c = start_col;
                }
                else if (strcmp(expr.value[0], "q") == 0)
                {
                    return 0;
                }
                else
                {
                    strcpy(status, "unrecognized cmd");
                }
                start_time = time(NULL);
                if (r != nrows)
                    start_row = r;
                if (c != ncols)
                    start_col = c;

                display_sheet(sheet, nrows, ncols, start_row, start_col, print_output);

                end_time = time(NULL);
                elapsed_time = (double)(end_time - start_time);
            }
            else if (strcmp(expr.type, "constant") == 0)
            {
                int val = string_to_int(expr.value[0]);
                char col_label[100];
                int r, c;

                extract_column_row(cell, col_label, &r);
                c = column_label_to_index(col_label);

                start_time = time(NULL);
                if (r >= nrows || c >= ncols)
                {
                    strcpy(status, "Invalid range");
                }
                else
                {
                    clear_dependencies(&sheet[r][c]);
                    sheet[r][c].error = false;
                    sheet[r][c].value = val;
                    strcpy(status, "ok");
                }

                // Clear old expression data and copy the new expression.
                memset(sheet[r][c].cellexpr, 0, sizeof(Expression));
                memcpy(sheet[r][c].cellexpr, &expr, sizeof(Expression));
                // (Optional) Ensure unused fields are empty:
                sheet[r][c].cellexpr->value[1][0] = '\0';
                sheet[r][c].cellexpr->operator[0] = '\0';
                sheet[r][c].cellexpr->function[0] = '\0';
                sheet[r][c].cellexpr->range[0] = '\0';

                // Recalculate dependents of this cell so that any arithmetic cells using A1 update.
                recalculate_cell(sheet, nrows, ncols, &sheet[r][c]);

                // printf("type: %s\n", sheet[r][c].cellexpr->type);
                // print_expression(sheet[r][c].cellexpr);
                display_sheet(sheet, nrows, ncols, start_row, start_col, print_output);

                end_time = time(NULL);
                elapsed_time = (double)(end_time - start_time);
            }

            else if (strcmp(expr.type, "arithmetic") == 0 || strcmp(expr.type, "cell") == 0)
            {
                char col_label[100];
                int r, c;
                extract_column_row(cell, col_label, &r);
                c = column_label_to_index(col_label);
                start_time = time(NULL);

                if (r >= nrows || c >= ncols)
                {
                    strcpy(status, "Invalid range");
                }
                else
                {
                    strcpy(status, "ok");

                    // Clear old dependency links from this cell.
                    clear_dependencies(&sheet[r][c]); // to check

                    int x = 0, y = 0;
                    bool cycleDetected = false;
                    Node *stack[100];
                    int stackSize = 0;

                    // Process first operand.
                    if (is_valid_cell_reference(expr.value[0]))
                    {
                        char col_label_dep1[100];
                        int rdep1, cdep1;
                        extract_column_row(expr.value[0], col_label_dep1, &rdep1);
                        cdep1 = column_label_to_index(col_label_dep1);
                        Node *dep1 = &sheet[rdep1][cdep1];
                        // printf("helpppp\n");

                        if (detect_cycle(dep1, &sheet[r][c], stack, stackSize))
                        {
                            cycleDetected = true;
                        }
                        else
                        {
                            // Need to do for cycle detect case also but skipping for now:)
                            x = sheet[rdep1][cdep1].value;
                        }
                        add_dependency(&sheet[r][c], dep1);
                        add_dependents(&sheet[r][c], dep1);
                    }
                    else
                    {
                        x = string_to_int(expr.value[0]);
                    }

                    // Reset stack for second operand.
                    stackSize = 0;
                    if(strcmp(expr.type,"cell")==0 ){
                        strcpy(expr.operator,"+");
                        strcpy(expr.value[1], "0");
                        // print_expression(&expr);

                    }
                    if (is_valid_cell_reference(expr.value[1]))
                    {
                        char col_label_dep2[100];
                        int rdep2, cdep2;
                        extract_column_row(expr.value[1], col_label_dep2, &rdep2);
                        cdep2 = column_label_to_index(col_label_dep2);
                        Node *dep2 = &sheet[rdep2][cdep2];

                        if (detect_cycle(dep2, &sheet[r][c], stack, stackSize))
                        {
                            cycleDetected = true;
                        }
                        else
                        {
                            // Need to do for cycle detect case also but skipping for now:)
                            y = sheet[rdep2][cdep2].value;
                        }
                        add_dependency(&sheet[r][c], dep2);
                        add_dependents(&sheet[r][c], dep2);
                        // printf("Finally error %d\n", dep2->value);
                    }
                    else
                    {
                        y = string_to_int(expr.value[1]);
                    }

                    if (cycleDetected)
                    {
                        sheet[r][c].error = true;
                        strcpy(sheet[r][c].expression, "ERR");
                        // printf("ERR: Finally CYCLEDETECTED\n");
                        for (int i = 0; i < sheet[r][c].dept_count; i++)
                        {
                            if (sheet[r][c].dependents[i])
                                recalculate_cell(sheet, nrows, ncols, sheet[r][c].dependents[i]);
                        }
                    }
                    else
                    {
                        char op = expr.operator[0];
                        if (op == '/' && y == 0)
                        {
                            sheet[r][c].error = true;
                            strcpy(sheet[r][c].expression, "ERR");
                        }
                        else
                        {
                            sheet[r][c].error = false;
                            sheet[r][c].value = performOpr(x, y, op);
                        }
                    }
                }

                // Overwrite the cell's expression with the new arithmetic expression.
                memset(sheet[r][c].cellexpr, 0, sizeof(Expression));
                memcpy(sheet[r][c].cellexpr, &expr, sizeof(Expression));

                // Propagate recalculation so dependents update.
                // printf("Calling recalculate_cell for %d:%d\n", r, c);
                if (!sheet[r][c].error)
                    recalculate_cell(sheet, nrows, ncols, &sheet[r][c]);

                // printf("type: %s\n", sheet[r][c].cellexpr->type);
                // print_expression(sheet[r][c].cellexpr);
                display_sheet(sheet, nrows, ncols, start_row, start_col, print_output);

                end_time = time(NULL);
                elapsed_time = (double)(end_time - start_time);
            }

            else if (strcmp(expr.type, "function") == 0)
            {
                char col_label[100];
                int r, c;
                extract_column_row(cell, col_label, &r);
                c = column_label_to_index(col_label);

                Node *stack[100];
                int stackSize = 0;

                start_time = time(NULL);
                if (r >= nrows || c >= ncols)
                {
                    strcpy(status, "Invalid range");
                }
                else if (strcmp(expr.function, "SLEEP") == 0)
                {
                    // Clear existing dependencies
                    clear_dependencies(&sheet[r][c]);
                    // int num;
                    // Check if the argument is a cell reference
                    if (is_valid_cell_reference(expr.range))
                    {
                        char col_label[10];
                        int arg_row, arg_col;
                        extract_column_row(expr.range, col_label, &arg_row);
                        arg_col = column_label_to_index(col_label);


                        if(detect_cycle(&sheet[arg_row][arg_col], &sheet[r][c] , stack, stackSize))
                        {
                            printf("Adding depend: arg_row: %d, arg_col: %d\n", arg_row, arg_col);

                            sheet[r][c].error = true;
                            sheet[arg_row][arg_col].error = true;
                            sheet[r][c].value = 0;
                            strcpy(sheet[r][c].expression, "ERR");
                            strcpy(sheet[arg_row][arg_col].expression, "ERR");
                            // continue;
                        }

                        // Add dependency
                        add_dependency(&sheet[r][c], &sheet[arg_row][arg_col]);
                        add_dependents(&sheet[r][c], &sheet[arg_row][arg_col]);
                    }

                }
                else
                {
                    strcpy(status, "ok");
                    int row1, col1, row2, col2;
                    if (is_valid_range(expr.range, &row1, &col1, &row2, &col2))
                    {
                        // For each cell in the range, add it as a dependency for the target cell,
                        // and add the target cell as a dependent to that cell.
                        for (int i = row1; i <= row2; i++)
                        {
                            for (int j = col1; j <= col2; j++)
                            {
                                add_dependency(&sheet[r][c], &sheet[i][j]);
                                add_dependents(&sheet[r][c], &sheet[i][j]);
                            }
                        }
                        // Evaluate the function based on its type.
                        if (strcmp(expr.function, "MIN") == 0)
                        {
                            sheet[r][c].value = min_range(sheet, row1, col1, row2, col2);
                        }
                        else if (strcmp(expr.function, "MAX") == 0)
                        {
                            sheet[r][c].value = max_range(sheet, row1, col1, row2, col2);
                        }
                        else if (strcmp(expr.function, "SUM") == 0)
                        {
                            sheet[r][c].value = sum_range(sheet, row1, col1, row2, col2);
                        }
                        else if (strcmp(expr.function, "AVG") == 0)
                        {
                            sheet[r][c].value = avg_range(sheet, row1, col1, row2, col2);
                        }
                        else if (strcmp(expr.function, "STDEV") == 0)
                        {
                            sheet[r][c].value = stdev_range(sheet, row1, col1, row2, col2);
                        }
                        else
                        {
                            strcpy(status, "unrecognized function");
                            sheet[r][c].error = true;
                        }
                        sheet[r][c].error = false;
                    }
                    else
                    {
                        sheet[r][c].error = true;
                        strcpy(sheet[r][c].expression, "ERR");
                    }
                }

                // Clear old expression data and copy the new function expression.
                memset(sheet[r][c].cellexpr, 0, sizeof(Expression));
                memcpy(sheet[r][c].cellexpr, &expr, sizeof(Expression));

                // Propagate changes so that any dependents update.
                recalculate_cell(sheet, nrows, ncols, &sheet[r][c]);

                // printf("type: %s\n", sheet[r][c].cellexpr->type);
                // print_expression(sheet[r][c].cellexpr);
                display_sheet(sheet, nrows, ncols, start_row, start_col, print_output);

                end_time = time(NULL);
                elapsed_time = (double)(end_time - start_time);
            }
        }
        else
        {
            bool flag = false;
            if (strcmp(input, "disable_output") == 0)
            {
                strcpy(status, "ok");
            }
            else if (strcmp(input, "enable_output") == 0)
            {
                strcpy(status, "ok");
                flag = true;
                print_output = false;
            }
            else if (strncmp(input, "scroll_to ", 10) == 0)
            {
                char cell[100];
                extract_cell(input, cell);
                if (is_valid_cell_reference(cell))
                {
                    char col_label[100];
                    int r, c;
                    extract_column_row(cell, col_label, &r);
                    c = column_label_to_index(col_label);

                    if (r >= nrows || c >= ncols)
                    {
                        strcpy(status, "Invalid range");
                    }
                    else
                    {
                        start_row = r;
                        start_col = c;
                    }
                }
                else
                {
                    strcpy(status, "Invalid range");
                }
            }
            else
            {
                strcpy(status, "unrecognized cmd");
            }

            start_time = time(NULL);

            display_sheet(sheet, nrows, ncols, start_row, start_col, print_output);

            end_time = time(NULL);
            elapsed_time = (double)(end_time - start_time);

            if (flag)
                print_output = true;
        }
    }

    for (int i = 0; i < nrows; i++)
    {
        free(sheet[i]);
    }
    free(sheet);

    return 0;
}
