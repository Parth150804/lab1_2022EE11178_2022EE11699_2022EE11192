#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

int main() {
    regex_t regex;
    int ret;
    const char* pattern = "^\\s*([-+]?[0-9]+|[A-Z]{1,3}[1-9][0-9]{0,2})\\s*([-+*/])\\s*([-+]?[0-9]+|[A-Z]{1,3}[1-9][0-9]{0,2})\\s*$";
    // const char* pattern = "^\\s*([A-Z]{1,3}[1-9][0-9]{0,2})\\s*$";
    // Compile the regex
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        char errbuf[100];
        regerror(ret, &regex, errbuf, sizeof(errbuf));
        fprintf(stderr, "Regex compilation failed: %s\n", errbuf);
        return 1; // Exit if regex compilation fails
    }

    printf("Regex compiled successfully.\n");

    // Test strings (must end with NULL)
    const char* test_strings[] = {
        // "-1*-1",
        // "-1*+1",
        // "-1+-1",
        // "-1-+1", // Should not match
        // "-1/+1",  // Should not match
        // "-1/-1", // Should not match
        "  ZZZ999   ",
        NULL // Terminate the array with NULL
    };

    // Test each string
    for (int i = 0; test_strings[i] != NULL; i++) {
        printf("Testing string: \"%s\"\n", test_strings[i]); // Debug print
        ret = regexec(&regex, test_strings[i], 0, NULL, 0);
        if (ret == 0) {
            printf("Match: %s\n", test_strings[i]);
        } else if (ret == REG_NOMATCH) {
            printf("No match: %s\n", test_strings[i]);
        } else {
            char errbuf[100];
            regerror(ret, &regex, errbuf, sizeof(errbuf));
            fprintf(stderr, "Regex execution failed: %s\n", errbuf);
        }
    }

    // printf("Testing string: \"%s\"\n", "10 * 2"); // Debug print
    // ret = regexec(&regex, "10 * 2", 0, NULL, 0);
    // if (ret == 0) {
    //     printf("Match: %s\n", "10 * 2");
    // } else if (ret == REG_NOMATCH) {
    //     printf("No match: %s\n", "10 * 2");
    // } else {
    //     char errbuf[100];
    //     regerror(ret, &regex, errbuf, sizeof(errbuf));
    //     fprintf(stderr, "Regex execution failed: %s\n", errbuf);
    // }

    // Free the compiled regex
    regfree(&regex);

    return 0;
}