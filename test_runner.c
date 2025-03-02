#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

void run_test_case(const char *input_file, const char *output_file) {
    int pipe_in[2], pipe_out[2];
    
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }
    
    FILE *infile = fopen(input_file, "r");
    if (!infile) {
        perror("Failed to open input file");
        return;
    }
    
    // Read the first line to get R and C
    int R, C;
    if (fscanf(infile, "%d %d\n", &R, &C) != 2) {
        fprintf(stderr, "Invalid format for rows and columns in input file\n");
        fclose(infile);
        return;
    }

    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process: Run `sheet` with R and C as arguments
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        
        close(pipe_in[1]);  // Close unused write end
        close(pipe_out[0]); // Close unused read end
        
        // Convert R and C to string arguments
        char r_str[10], c_str[10];
        snprintf(r_str, sizeof(r_str), "%d", R);
        snprintf(c_str, sizeof(c_str), "%d", C);
        
        execl("./sheet", "./sheet", r_str, c_str, NULL);
        perror("execl failed");
        exit(1);
    } else {
        // Parent process: Send input & capture output
        close(pipe_in[0]);  // Close unused read end
        close(pipe_out[1]); // Close unused write end

        // Send input commands to `sheet`
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        while ((read = getline(&line, &len, infile)) != -1) {
            write(pipe_in[1], line, read);
        }
        
        write(pipe_in[1], "q\n", 2);  // Quit command
        close(pipe_in[1]);
        fclose(infile);
        free(line);

        // Capture output
        FILE *pipe_out_fp = fdopen(pipe_out[0], "r");
        if (!pipe_out_fp) {
            perror("fdopen failed");
            return;
        }

        FILE *outfile = fopen(output_file, "w");
        if (!outfile) {
            perror("Failed to open output file");
            fclose(pipe_out_fp);
            return;
        }

        // Read output line by line and write exactly as received
        char *output_line = NULL;
        size_t out_len = 0;
        while (getline(&output_line, &out_len, pipe_out_fp) != -1) {
            fprintf(outfile, "%s", output_line);
        }

        free(output_line);
        fclose(outfile);
        fclose(pipe_out_fp);
        
        wait(NULL); // Wait for child process to finish
    }
}

int main() {
    printf("Running Spreadsheet Test...\n");
    run_test_case("input.txt", "output.txt");
    printf("Test completed. Check output.txt for results.\n");
    return 0;
}
