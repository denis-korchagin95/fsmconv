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

$(OBJ)visualize.o: $(SRC)visualize.c $(SRC)visualize.h $(SRC)nfa_types.h
	$(CC) $(CFLAGS) -c $(SRC)visualize.c -o $(OBJ)visualize.o

$(OBJ)util.o: $(SRC)util.c $(SRC)util.h
	$(CC) $(CFLAGS) -c $(SRC)util.c -o $(OBJ)util.o

OBJECTS  = $(OBJ)allocator.o
OBJECTS += $(OBJ)debug.o
OBJECTS += $(OBJ)parser.o
OBJECTS += $(OBJ)nfa_compiler.o
OBJECTS += $(OBJ)visualize.o
OBJECTS += $(OBJ)util.o
OBJECTS += $(OBJ)main.o


build: $(OBJECTS)
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
