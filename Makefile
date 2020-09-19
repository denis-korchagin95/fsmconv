CC=gcc
CFLAGS=-g -Wall -std=c89
LFLAGS=
SRC=./
BIN=./bin/
OBJ=./obj/
PROGRAM=fsmconv
INSTALL_PATH=/usr/local/bin/

SAMPLE_FILE=./examples/simple_nfa1.txt

vpath %.c $(SRC)
vpath %.h $(SRC)

all: $(BIN)$(PROGRAM) run

OBJECTS = allocator.o debug.o parser.o fsm_compiler.o visualize.o util.o \
	  fsm.o fsm_state.o fsm_state_list.o fsm_transition.o character_list.o main.o

$(BIN)$(PROGRAM): $(addprefix $(OBJ), $(OBJECTS))
	@$(CC) $(LFLAGS) $^ -o $@

run:
	$(BIN)$(PROGRAM) $(SAMPLE_FILE)

install: build
	@cp $(BIN)$(PROGRAM) $(INSTALL_PATH)$(PROGRAM)

uninstall:
	@rm -v $(INSTALL_PATH)$(PROGRAM)

clean:
	@rm -rfv $(BIN)$(PROGRAM)
	@rm -rfv $(OBJ)*
	@rm -rfv nfa.dot
	@rm -rfv dfa.dot
	@rm -rfv nfa.svg
	@rm -rfv dfa.svg

visualize:
	dot -Tsvg $(SRC)nfa.dot -o nfa.svg
	dot -Tsvg $(SRC)dfa.dot -o dfa.svg

$(OBJ)%.o: %.c
	$(CC) $(CFLAGS) -c $(SRC)$*.c -o $@

$(OBJ)allocator.o: allocator.c character_list.h fsm_types.h parser_types.h allocator.h
$(OBJ)character_list.o: character_list.c fsm_types.h util.h character_list.h
$(OBJ)debug.o: debug.c parser.h parser_types.h fsm_types.h fsm_state_list.h debug.h
$(OBJ)fsm.o: fsm.c character_list.h fsm_types.h parser_types.h fsm.h allocator.h util.h fsm_state.h fsm_state_list.h
$(OBJ)fsm_compiler.o: fsm_compiler.c character_list.h fsm_types.h parser_types.h parser.h allocator.h util.h fsm.h fsm_state.h
$(OBJ)fsm_state.o: fsm_state.c fsm_types.h fsm_state.h
$(OBJ)fsm_state_list.o: fsm_state_list.c character_list.h fsm_types.h parser_types.h fsm_state_list.h allocator.h
$(OBJ)fsm_transition.o: fsm_transition.c fsm_types.h fsm_transition.h
$(OBJ)parser.o: parser.c character_list.h fsm_types.h parser.h parser_types.h allocator.h debug.h
$(OBJ)util.o: util.c util.h fsm_types.h
$(OBJ)visualize.o: visualize.c fsm_types.h util.h visualize.h fsm.h
$(OBJ)main.o: main.c parser.h fsm_compiler.h visualize.h fsm_types.h fsm.h util.h
