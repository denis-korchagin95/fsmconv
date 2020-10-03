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

OBJECTS=allocator.o internal_allocators.o parser.o tokenizer.o fsm_compiler.o symbol.o stream.o main.o \
	util.o fsm.o fsm_state.o fsm_state_list.o fsm_transition.o character_list.o visualize.o

TEST_PARSING_OBJECTS=test-parsing.o tokenizer.o parser.o stream.o allocator.o internal_allocators.o symbol.o debug.o util.o
TEST_TOKENIZER_OBJECTS=test-tokenizer.o debug.o tokenizer.o stream.o allocator.o internal_allocators.o symbol.o parser.o

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
	@rm -rfv $(BIN)test-tokenizer
	@rm -rfv $(BIN)test-parsing
	@rm -rfv $(OBJ)*
	@rm -rfv nfa.dot
	@rm -rfv dfa.dot
	@rm -rfv nfa.svg
	@rm -rfv dfa.svg

test-tokenizer: $(addprefix $(OBJ), $(TEST_TOKENIZER_OBJECTS))
	$(CC) $(LFLAGS) $^ -o $(BIN)test-tokenizer

test-parsing: $(addprefix $(OBJ), $(TEST_PARSING_OBJECTS))
	$(CC) $(LFLAGS) $^ -o $(BIN)test-parsing

visualize:
	dot -Tsvg $(SRC)nfa.dot -o nfa.svg
	dot -Tsvg $(SRC)dfa.dot -o dfa.svg

$(OBJ)%.o: %.c
	$(CC) $(CFLAGS) -c $(SRC)$*.c -o $@

$(OBJ)test-parsing.o: test-parsing.c internal_allocators.h allocator.h stream.h parser.h tokenizer.h debug.h
$(OBJ)test-tokenizer.o: test-tokenizer.c tokenizer.h stream.h debug.h internal_allocators.h allocator.h symbol.h parser.h

-include $(DEPENDENCIES_FILE)
