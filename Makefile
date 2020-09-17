CC=gcc
CFLAGS=-g -Wall -std=c89
LFLAGS=
SRC=./
BIN=./bin/
OBJ=./obj/
PROGRAM=fsmconv

SAMPLE_FILE=./examples/simple_nfa1.txt

all: build run

$(OBJ)debug.o: $(SRC)debug.c $(SRC)debug.h $(SRC)parser.h $(SRC)parser_types.h $(SRC)fsm_types.h $(SRC)fsm_state_list.h
	$(CC) $(CFLAGS) -c $(SRC)debug.c -o $(OBJ)debug.o

$(OBJ)main.o: $(SRC)main.c $(SRC)parser.h $(SRC)allocator.h $(SRC)debug.h $(SRC)fsm_types.h $(SRC)util.h $(SRC)parser_types.h $(SRC)fsm_compiler.h $(SRC)fsm.h
	$(CC) $(CFLAGS) -c $(SRC)main.c -o $(OBJ)main.o

$(OBJ)parser.o: $(SRC)parser.c $(SRC)parser.h $(SRC)parser_types.h $(SRC)allocator.h $(SRC)debug.h $(SRC)fsm_types.h
	$(CC) $(CFLAGS) -c $(SRC)parser.c -o $(OBJ)parser.o

$(OBJ)allocator.o: $(SRC)allocator.c $(SRC)allocator.h $(SRC)fsm_types.h $(SRC)parser_types.h
	$(CC) $(CFLAGS) -c $(SRC)allocator.c -o $(OBJ)allocator.o

$(OBJ)fsm_compiler.o: $(SRC)fsm_compiler.c $(SRC)fsm_types.h $(SRC)parser_types.h $(SRC)allocator.h $(SRC)fsm.h $(SRC)fsm_state.h $(SRC)parser.h
	$(CC) $(CFLAGS) -c $(SRC)fsm_compiler.c -o $(OBJ)fsm_compiler.o

$(OBJ)visualize.o: $(SRC)visualize.c $(SRC)visualize.h $(SRC)fsm_types.h
	$(CC) $(CFLAGS) -c $(SRC)visualize.c -o $(OBJ)visualize.o

$(OBJ)util.o: $(SRC)util.c $(SRC)util.h
	$(CC) $(CFLAGS) -c $(SRC)util.c -o $(OBJ)util.o

$(OBJ)fsm.o: $(SRC)fsm.c $(SRC)fsm.h $(SRC)fsm_types.h $(SRC)parser_types.h $(SRC)allocator.h $(SRC)util.h $(SRC)fsm_state.h $(SRC)fsm_state_list.h $(SRC)character_list.h
	$(CC) $(CFLAGS) -c $(SRC)fsm.c -o $(OBJ)fsm.o

$(OBJ)fsm_state.o: $(SRC)fsm_state.c $(SRC)fsm_state.h $(SRC)fsm_types.h
	$(CC) $(CFLAGS) -c $(SRC)fsm_state.c -o $(OBJ)fsm_state.o

$(OBJ)fsm_state_list.o: $(SRC)fsm_state_list.c $(SRC)fsm_state_list.h $(SRC)fsm_types.h $(SRC)parser_types.h $(SRC)allocator.h
	$(CC) $(CFLAGS) -c $(SRC)fsm_state_list.c -o $(OBJ)fsm_state_list.o

$(OBJ)fsm_transition.o: $(SRC)fsm_transition.c $(SRC)fsm_transition.h $(SRC)fsm_types.h
	$(CC) $(CFLAGS) -c $(SRC)fsm_transition.c -o $(OBJ)fsm_transition.o

$(OBJ)character_list.o: $(SRC)character_list.c $(SRC)character_list.h $(SRC)fsm_types.h $(SRC)util.h
	$(CC) $(CFLAGS) -c $(SRC)character_list.c -o $(OBJ)character_list.o

OBJECTS  = $(OBJ)allocator.o
OBJECTS += $(OBJ)debug.o
OBJECTS += $(OBJ)parser.o
OBJECTS += $(OBJ)fsm_compiler.o
OBJECTS += $(OBJ)visualize.o
OBJECTS += $(OBJ)util.o
OBJECTS += $(OBJ)fsm.o
OBJECTS += $(OBJ)fsm_state.o
OBJECTS += $(OBJ)fsm_state_list.o
OBJECTS += $(OBJ)fsm_transition.o
OBJECTS += $(OBJ)character_list.o
OBJECTS += $(OBJ)main.o

build: $(OBJECTS)
	$(CC) $(LFLAGS) $^ -o $(BIN)$(PROGRAM)

run:
	$(BIN)$(PROGRAM) $(SAMPLE_FILE)

clean:
	rm -rf $(BIN)$(PROGRAM)
	rm -rf $(OBJ)*
	rm -rf nfa.dot
	rm -rf dfa.dot
	rm -rf nfa.svg
	rm -rf dfa.svg

visualize:
	dot -Tsvg $(SRC)nfa.dot -o nfa.svg
	dot -Tsvg $(SRC)dfa.dot -o dfa.svg

