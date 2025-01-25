CC = gcc
CFLAGS = -Wall -std=c11
TARGET = test/testing_input_handling
SRC = test/testing_input_handling.c
INPUT_FILE = test/test_inputs.txt

.PHONY: all clean test_input_handling clean_input_validation

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

test_input_handling: $(TARGET)
	./$(TARGET) $(INPUT_FILE)

clean_input_handling:
	rm -f $(TARGET)
