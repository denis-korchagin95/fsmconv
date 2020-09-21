CC=gcc
CFLAGS=-g -Wall -std=c89
LFLAGS=
SRC=./
BIN=./bin/
OBJ=./obj/
PROGRAM=fsmconv
INSTALL_PATH=/usr/local/bin/
DEPENDENCIES_FILE=dependencies.mk

SAMPLE_FILE=./examples/simple_nfa1.txt

vpath %.c $(SRC)
vpath %.h $(SRC)

all: build run

OBJECTS = allocator.o debug.o parser.o fsm_compiler.o visualize.o util.o \
	  fsm.o fsm_state.o fsm_state_list.o fsm_transition.o character_list.o main.o

build: $(addprefix $(OBJ), $(OBJECTS)) | $(DEPENDENCIES_FILE)
	@$(CC) $(LFLAGS) $^ -o $(BIN)$(PROGRAM)

$(DEPENDENCIES_FILE):
	@rm -rf $(DEPENDENCIES_FILE)
	@$(foreach file, $(OBJECTS), $(CC) -MT $(OBJ)$(file) -MM $(patsubst %.o, %.c, $(file)) >> $(DEPENDENCIES_FILE);)

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

-include $(DEPENDENCIES_FILE)
