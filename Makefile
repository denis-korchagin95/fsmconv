CC=gcc
CFLAGS=-g -Wall -std=c89
LFLAGS=
SRC=./
BIN=./bin/
OBJ=./obj/
PROGRAM=nfa2dfaconv

all: build run

$(OBJ)debug.o: $(SRC)debug.c $(SRC)debug.h $(SRC)parser.h $(SRC)parser_types.h
	$(CC) $(CFLAGS) -c $(SRC)debug.c -o $(OBJ)debug.o

$(OBJ)main.o: $(SRC)main.c $(SRC)parser.h $(SRC)allocator.h $(SRC)debug.h $(SRC)nfa_types.h $(SRC)util.h $(SRC)parser_types.h $(SRC)nfa_compiler.h
	$(CC) $(CFLAGS) -c $(SRC)main.c -o $(OBJ)main.o

$(OBJ)parser.o: $(SRC)parser.c $(SRC)parser.h $(SRC)parser_types.h $(SRC)allocator.h $(SRC)debug.h $(SRC)nfa_types.h
	$(CC) $(CFLAGS) -c $(SRC)parser.c -o $(OBJ)parser.o

$(OBJ)allocator.o: $(SRC)allocator.c $(SRC)allocator.h $(SRC)nfa_types.h $(SRC)parser_types.h
	$(CC) $(CFLAGS) -c $(SRC)allocator.c -o $(OBJ)allocator.o

$(OBJ)nfa_compiler.o: $(SRC)nfa_compiler.c $(SRC)nfa_types.h $(SRC)parser_types.h $(SRC)allocator.h
	$(CC) $(CFLAGS) -c $(SRC)nfa_compiler.c -o $(OBJ)nfa_compiler.o

build: $(OBJ)allocator.o $(OBJ)debug.o $(OBJ)parser.o $(OBJ)nfa_compiler.o $(OBJ)main.o
	$(CC) $(LFLAGS) $^ -o $(BIN)$(PROGRAM)

clean:
	rm -rf $(BIN)$(PROGRAM)
	rm -rf $(OBJ)*
	rm -rf nfa.dot
	rm -rf dfa.dot
	rm -rf nfa.svg
	rm -rf dfa.svg

run:
	$(BIN)$(PROGRAM)
	dot -Tsvg $(SRC)nfa.dot -o nfa.svg
	dot -Tsvg $(SRC)dfa.dot -o dfa.svg
