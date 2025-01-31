#include <stdlib.h>
#include <stdio.h> // Input and Output
#include <string.h> // String
#include <regex.h> // REGEX POSIX.2 
#include <stdbool.h> // Boolean type
#include <ctype.h> 

// Struct to store parsed expressions
typedef struct {
    char type[12];     // "control", "constant", "cell", "arithmetic", "function"
    char value[2][16]; // "w, a, s, d, q", "constant", "cell", "arithmetic"
    char operator[2];  // +, -, *, /
    char function[8];  // MIN, MAX, AVG, SUM, STDEV, SLEEP
    char range[16];    // ZZZ999:ZZZ999
} Expression;

// Function prototypes
bool parse_input(const char* input, char* cell, Expression* expr);
bool validate_control_input(const char* input, Expression* expr);
bool validate_formula_input(const char* input, char* cell, Expression* expr);
bool validate_cell(const char* input, char* cell);
bool validate_expression(const char* input, Expression* expr);
bool validate_range(const char* range) ;

// Helper function to compile and execute regex
static bool executeRegex(const char* pattern, const char* input, regmatch_t* matches, int match_count, char* task) { //DONE
    regex_t regex; int ret;

    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if(ret != 0){
        char errbuf[100];
        regerror(ret, &regex, errbuf, sizeof(errbuf));
        fprintf(stderr, "%s regex compilation failed: %s", task, errbuf);
        return false;
    }

    ret = regexec(&regex, input, match_count, matches, 0);
    if (ret != 0){
        if(ret != REG_NOMATCH){
            char errbuf[100];
            regerror(ret, &regex, errbuf, sizeof(errbuf));
            fprintf(stderr, "%s regex execution failed: %s", task, errbuf);
        }
        regfree(&regex);
        return false;
    }
    regfree(&regex);
    return true;
}

// Helper function to assign regex matches
void assign_matches(const char* input, regmatch_t* match, char* output, size_t size) { //DONE
    size_t match_len = match->rm_eo - match->rm_so;
    if (match_len < size) {
        strncpy(output, input + match->rm_so, match_len);
        output[match_len] = '\0';
    }
}

// Parses the input and determines its type
bool parse_input(const char* input, char* cell, Expression* expr){ //DONE
    return validate_control_input(input, expr) || validate_formula_input(input, cell, expr);
}

// Validates control input (e.g., "w", "a", "s", "d", "q")
bool validate_control_input(const char* input, Expression* expr){ //DONE
    const char* control_pattern = "^\\s*([wasdq])\\s*$";
    regmatch_t matches[2];

    if (!executeRegex(control_pattern, input, matches, 2, "Control")) {
        return false;
    }

    strncpy(expr->type, "control", sizeof(expr->type) - 1);
    expr->type[sizeof(expr->type) - 1] = '\0'; 
    
    expr->value[0][0] = input[matches[1].rm_so];
    expr->value[0][1] = '\0';
    return true;
}

// Validates formula input in the format "cell = expression"
bool validate_formula_input(const char* input, char* cell, Expression* expr){ //DONE
    const char* formula_pattern = "^\\s*([^=]+)\\s*=\\s*(.+)$";
    regmatch_t matches[3];

    if (!executeRegex(formula_pattern, input, matches, 3, "Formula")) {
        // fprintf(stderr, ". Invalid Formula: %s\n", input);
        return false;
    }

    // Extract left-hand side (cell reference)
    char tempCell[16];
    assign_matches(input, &matches[1], tempCell, sizeof(tempCell));
    if(!validate_cell(tempCell, cell)){
        return false;
    }

    // Extract right-hand side (expression)
    char tempExpression[256];
    assign_matches(input, &matches[2], tempExpression, sizeof(tempExpression));
    if(!validate_expression(tempExpression, expr)){
        return false;
    }

    return true;
}

// Validates cell references (e.g., "A1", "ZZ99")
bool validate_cell(const char* input, char* cell){ //DONE
    const char* cell_pattern = "^\\s*([A-Z]{1,3}[1-9][0-9]{0,2})\\s*$";
    regmatch_t matches[2];

    if (!executeRegex(cell_pattern, input, matches, 2, "Cell")) {
        fprintf(stderr, ". Invalid cell: %s. Size: %ld\n", input, sizeof(input));
        return false;
    }

    assign_matches(input, &matches[1], cell, sizeof(cell));
    return true;
}

bool validate_expression(const char* input, Expression* expr){
    const char* constant_pattern = "^\\s*([-+]?[0-9]+)\\s*$";
    const char* positive_constant_pattern = "^\\s*(\\+?[0-9]+)\\s*$";
    const char* cell_pattern = "^\\s*([A-Z]{1,3}[1-9][0-9]{0,2})\\s*$";
    const char* arithmetic_pattern = "^\\s*([-+]?[0-9]+|[A-Z]{1,3}[1-9][0-9]{0,2})\\s*([-+*/])\\s*([-+]?[0-9]+|[A-Z]{1,3}[1-9][0-9]{0,2})\\s*$";
    const char* function_pattern = "^\\s*(MIN|MAX|AVG|SUM|STDEV|SLEEP)\\s*\\(\\s*([^)]+)\\s*\\)\\s*$";

    regmatch_t matches[4];

    // Check if the expression is a constant
    if (executeRegex(constant_pattern, input, matches, 2, "Constant")) {
        strncpy(expr->type, "constant", sizeof(expr->type) - 1);
        expr->type[sizeof(expr->type) - 1] = '\0';

        assign_matches(input, &matches[1], expr->value[0], sizeof(expr->value[0]));
        return true;
    }

    // Check if the expression is a cell reference
    if (executeRegex(cell_pattern, input, matches, 2, "Cell")) {
        strncpy(expr->type, "cell", sizeof(expr->type) - 1);
        expr->type[sizeof(expr->type) - 1] = '\0';

        assign_matches(input, &matches[1], expr->value[0], sizeof(expr->value[0]));
        return true;
    }

    // Check if the expression is an arithmetic operation
    if (executeRegex(arithmetic_pattern, input, matches, 4, "Arithmetic")) {
        strncpy(expr->type, "arithmetic", sizeof(expr->type) - 1);
        expr->type[sizeof(expr->type) - 1] = '\0';

        assign_matches(input, &matches[1], expr->value[0], sizeof(expr->value[0])); // First value
        assign_matches(input, &matches[2], expr->operator, sizeof(expr->operator)); // Operator
        assign_matches(input, &matches[3], expr->value[1], sizeof(expr->value[1])); // Second value
        return true;
    }

    // Check if the expression is a function
    if (executeRegex(function_pattern, input, matches, 4, "Function")) {
        strncpy(expr->type, "function", sizeof(expr->type) - 1);
        expr->type[sizeof(expr->type) - 1] = '\0';

        assign_matches(input, &matches[1], expr->function, sizeof(expr->function)); // Function name
        assign_matches(input, &matches[2], expr->range, sizeof(expr->range)); // Range or value

        // Validate the range or value inside the function
        if (strcmp(expr->function, "SLEEP") == 0) {
            // SLEEP function expects a constant or cell reference
            if (!executeRegex(positive_constant_pattern, expr->range, matches, 2, "SLEEP Value") && !executeRegex(cell_pattern, expr->range, matches, 2, "SLEEP Cell")) {
                fprintf(stderr, ". Invalid SLEEP argument: %s\n", expr->range);
                return false;
            }
        } else {
            // Other functions expect a valid range
            if (!validate_range(expr->range)) {
                fprintf(stderr, ". Invalid range: %s\n", expr->range);
                return false;
            }
        }
        return true;
    }

    // If none of the above patterns match, the expression is invalid
    fprintf(stderr, ". Invalid expression: %s\n", input);
    return false;
}

int column_to_index(const char* column) {
    int index = 0;
    while (*column) {
        index = index * 26 + (toupper(*column) - 'A' + 1);
        column++;
    }
    return index;
}

bool validate_range(const char* range) {
    const char* range_pattern = "^\\s*([A-Z]{1,3}[1-9][0-9]{0,2})\\s*:\\s*([A-Z]{1,3}[1-9][0-9]{0,2})\\s*$";
    regmatch_t matches[3];

    if (!executeRegex(range_pattern, range, matches, 3, "Range")) {
        return false;
    }

    // Extract the start and end cells
    char start_cell[16], end_cell[16];
    assign_matches(range, &matches[1], start_cell, sizeof(start_cell));
    assign_matches(range, &matches[2], end_cell, sizeof(end_cell));

    // Extract column and row from start cell
    char* start_col_end = start_cell;
    while (isalpha(*start_col_end)) start_col_end++;
    char start_col[16];
    strncpy(start_col, start_cell, start_col_end - start_cell);
    start_col[start_col_end - start_cell] = '\0';
    int start_row = atoi(start_col_end);

    // Extract column and row from end cell
    char* end_col_end = end_cell;
    while (isalpha(*end_col_end)) end_col_end++;
    char end_col[16];
    strncpy(end_col, end_cell, end_col_end - end_cell);
    end_col[end_col_end - end_cell] = '\0';
    int end_row = atoi(end_col_end);

    // Convert columns to indices
    int start_col_index = column_to_index(start_col);
    int end_col_index = column_to_index(end_col);

    // Validate that the start cell is before or equal to the end cell
    if (start_col_index > end_col_index || start_row > end_row) {
        return false;
    }

    return true;
}

int main() {
    FILE *input_file = fopen("test_inputs.txt", "r");
    FILE *output_file = fopen("test_results.txt", "w");

    if (!input_file || !output_file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    char input[256];
    char cell[16];
    Expression expr;

    fprintf(output_file, "%-30s | %-10s | %s\n", "Input", "Valid", "Parsed Information");
    fprintf(output_file, "------------------------------------------------------------\n");

    while (fgets(input, sizeof(input), input_file)) {
        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';

        // Skip empty lines and comment lines starting with '#'
        int is_comment = 0;
        int is_empty = 1;
        for (size_t i = 0; i < strlen(input); i++) {
            if (isspace(input[i])) continue;
            if (input[i] == '#') {
                is_comment = 1;
                break;
            }
            is_empty = 0;
            break;
        }
        if (is_comment || is_empty) {
            continue;  // Skip processing this line
        }

        // Initialize the Expression struct
        memset(&expr, 0, sizeof(Expression));

        // Parse and validate the input
        bool valid = parse_input(input, cell, &expr);

        printf("%s\n", expr.type);
        printf("%s\n", expr.value[0]);
        printf("%s\n", expr.value[1]);
        printf("%s\n", expr.operator);
        printf("%s\n", expr.function);
        printf("%s\n", expr.range);

        // Write results to the output file
        fprintf(output_file, "%-30s | %-10s | ", input, valid ? "Yes" : "No");

        if (valid) {
            if (strcmp(expr.type, "control") == 0) {
                fprintf(output_file, "Control: %s", expr.value[0]);
            } else if (strcmp(expr.type, "constant") == 0) {
                fprintf(output_file, "Constant: %s", expr.value[0]);
            } else if (strcmp(expr.type, "cell") == 0) {
                fprintf(output_file, "Cell Reference: %s", expr.value[0]);
            } else if (strcmp(expr.type, "arithmetic") == 0) {
                fprintf(output_file, "Arithmetic: %s %s %s", expr.value[0], expr.operator, expr.value[1]);
            } else if (strcmp(expr.type, "function") == 0) {
                fprintf(output_file, "Function: %s(%s)", expr.function, expr.range);
            }
        }

        fprintf(output_file, "\n");
    }

    fclose(input_file);
    fclose(output_file);

    printf("Testing complete. Results written to 'test_results.txt'.\n");
    return EXIT_SUCCESS;
}
