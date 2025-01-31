#include "commands.h"

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
