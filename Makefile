PDFLATEX=pdflatex
TEXFILE=report
CC = gcc
CFLAGS = -lm

all: sheet

sheet: v0.c
	@$(CC) v0.c -o sheet $(CFLAGS)

test: test_runner
	@echo "Running test cases..."
	@./test_runner

test_runner: test_runner.c
	@$(CC) test_runner.c -o test_runner

report: $(TEXFILE).tex
	$(PDFLATEX) $(TEXFILE).tex
	$(PDFLATEX) $(TEXFILE).tex  # Run twice to ensure proper references

clean:
	@rm -f sheet test_runner $(TEXFILE).aux $(TEXFILE).log $(TEXFILE).out $(TEXFILE).toc $(TEXFILE).pdf