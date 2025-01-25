#include <stdio.h>
#include <string.h>
#include <regex.h>

int parse_input(const char *input, char *cell, char *expression) {
    // Regular expression for validation and extraction
    const char *pattern = "^([A-Z]{1,3}[1-9][0-9]{0,2})\\s*=\\s*(.+)$";
    regex_t regex;
    regmatch_t matches[3]; // To capture Cell and Expression

    // Compile the regex
    if (regcomp(&regex, pattern, REG_EXTENDED)) {
        fprintf(stderr, "Could not compile regex\n");
        return 1; // Indicate failure
    }

    // Execute the regex on the input string
    if (regexec(&regex, input, 3, matches, 0) == 0) {
        // Extract matched groups
        size_t cell_len = matches[1].rm_eo - matches[1].rm_so;
        size_t expr_len = matches[2].rm_eo - matches[2].rm_so;

        strncpy(cell, input + matches[1].rm_so, cell_len);
        cell[cell_len] = '\0'; // Null-terminate the string

        strncpy(expression, input + matches[2].rm_so, expr_len);
        expression[expr_len] = '\0'; // Null-terminate the string

        regfree(&regex); // Free regex memory
        return 0;        // Indicate success
    } else {
        regfree(&regex); // Free regex memory
        return 1;        // Invalid format
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r"); // Open the input file provided as an argument
    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", argv[1]);
        return 1;
    }

    char input[200]; // To hold each input line
    char cell[50], expression[100];

    // Read each line from the file
    while (fgets(input, sizeof(input), file)) {
        // Remove newline character if present
        input[strcspn(input, "\n")] = 0;

        printf("Input: %s\n", input);
        if (parse_input(input, cell, expression) == 0) {
            printf("Valid! Cell: '%s', Expression: '%s'\n", cell, expression);
        } else {
            printf("Invalid input format!\n");
        }
    }

    fclose(file); // Close the file
    return 0;
}
