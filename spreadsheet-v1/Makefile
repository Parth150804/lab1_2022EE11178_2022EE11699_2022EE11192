# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c11

# Files
TEST_SRC = test/testing_input_handling.c
TARGET = test/testing_input_handling
INPUT_FILE = test/test_inputs.txt

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Rule to run tests
test: $(TARGET)
	./$(TARGET) $(INPUT_FILE)

# Rule to clean up generated files
clean:
	rm -f $(TARGET)

.PHONY: all test clean