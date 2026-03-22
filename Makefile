CC=gcc
CFLAGS=-g -Wall -std=c89
LFLAGS=
SRC=./
BIN=./bin/
OBJ=./obj/
TESTERS=./testers/
PROGRAM=fsmconv
INSTALL_PATH=/usr/local/bin/
DEPENDENCIES_FILE=dependencies.mk

SAMPLE_FILE=./examples/simple_nfa1.txt

vpath %.c $(SRC)
vpath %.h $(SRC)

all: build run

OBJECTS=allocator.o parser.o tokenizer.o fsm_compiler.o symbol.o stream.o main.o \
	util.o fsm.o fsm_state.o fsm_state_list.o fsm_transition.o visualize.o

PARSING_TESTER_OBJECTS=parsing-tester.o tokenizer.o parser.o stream.o allocator.o symbol.o print.o util.o
TOKENIZER_TESTER_OBJECTS=tokenizer-tester.o print.o tokenizer.o stream.o allocator.o symbol.o parser.o util.o

build: $(addprefix $(OBJ), $(OBJECTS)) | dependencies
	@$(CC) $(LFLAGS) $^ -o $(BIN)$(PROGRAM)

dependencies:
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
	@rm -rfv $(BIN)testers/
	@rm -rfv $(OBJ)*
	@rm -rfv nfa.dot
	@rm -rfv dfa.dot
	@rm -rfv nfa.svg
	@rm -rfv dfa.svg

test: build tokenizer-tester parsing-tester
	@jcunit --colors tests/

test-clean: clean test

tokenizer-tester: $(addprefix $(OBJ), $(TOKENIZER_TESTER_OBJECTS)) | $(BIN)testers
	@$(CC) $(LFLAGS) $^ -o $(BIN)testers/tokenizer-tester

parsing-tester: $(addprefix $(OBJ), $(PARSING_TESTER_OBJECTS)) | $(BIN)testers
	@$(CC) $(LFLAGS) $^ -o $(BIN)testers/parsing-tester

$(BIN)testers:
	@mkdir -p $@

visualize:
	dot -Tsvg $(SRC)nfa.dot -o nfa.svg
	dot -Tsvg $(SRC)dfa.dot -o dfa.svg

$(OBJ)%.o: %.c
	$(CC) $(CFLAGS) -c $(SRC)$*.c -o $@

$(OBJ)parsing-tester.o: $(TESTERS)parsing-tester.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

$(OBJ)tokenizer-tester.o: $(TESTERS)tokenizer-tester.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

-include $(DEPENDENCIES_FILE)
