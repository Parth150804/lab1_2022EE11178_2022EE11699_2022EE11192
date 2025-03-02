PDFLATEX=pdflatex
TEXFILE=report
CC = gcc
CFLAGS = -lm
TARGET_DIR = ./target/release

all: spreadsheet $(TARGET_DIR)/spreadsheet

$(TARGET_DIR):
	@mkdir -p $(TARGET_DIR)

spreadsheet: v0.c
	@$(CC) v0.c -o spreadsheet $(CFLAGS)

$(TARGET_DIR)/spreadsheet: v0.c | $(TARGET_DIR)
	@$(CC) v0.c -o $(TARGET_DIR)/spreadsheet $(CFLAGS)

test: test_runner
	@echo "Running test cases..."
	@./test_runner

test_runner: test_runner.c
	@$(CC) test_runner.c -o test_runner

report: $(TEXFILE).tex
	$(PDFLATEX) $(TEXFILE).tex
	$(PDFLATEX) $(TEXFILE).tex  # Run twice to ensure proper references

clean:
	@rm -f spreadsheet $(TARGET_DIR)/spreadsheet test_runner $(TEXFILE).aux $(TEXFILE).log $(TEXFILE).out $(TEXFILE).toc $(TEXFILE).pdf
