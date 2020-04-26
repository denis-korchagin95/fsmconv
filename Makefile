CC=gcc
CFLAGS=-g -Wall -std=c89
SRC=./
BIN=./bin/

all: build run

build: $(SRC)main.c
	$(CC) $(CFLAGS) $(SRC)main.c -o $(BIN)main

run:
	$(BIN)main
	dot -Tsvg $(SRC)nfa.dot -o nfa.svg
	dot -Tsvg $(SRC)dfa.dot -o dfa.svg
