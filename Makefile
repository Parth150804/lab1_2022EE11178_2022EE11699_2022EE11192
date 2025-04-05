all:
	mkdir -p ./target/release
	gcc v0.c -o ./target/release/spreadsheet -lm
