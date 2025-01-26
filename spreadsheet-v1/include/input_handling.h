#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <regex.h>    // REGEX POSIX.2
#include <stdbool.h> 
#include <ctype.h> 

// Forward declaration of the Expression struct
typedef struct Expression Expression;

// Define the struct
struct Expression {
    char type[12];     // "control", "constant", "cell", "arithmetic", "function"
    char value[2][16]; // "w, a, s, d, q", "constant", "cell", "arithmetic"
    char operator[2];  // +, -, *, /
    char function[8];  // MIN, MAX, AVG, SUM, STDEV, SLEEP
    char range[16];    // ZZZ999:ZZZ999
};

// Function prototypes
bool parse_input(const char* input, char* cell, Expression* expr);
bool validate_control_input(const char* input, Expression* expr);
bool validate_formula_input(const char* input, char* cell, Expression* expr);
bool validate_cell(const char* input, char* cell);
bool validate_expression(const char* input, Expression* expr);
bool validate_range(const char* range);

#endif // INPUT_HANDLING_H