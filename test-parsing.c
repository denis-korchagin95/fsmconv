#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "internal_allocators.h"
#include "stream.h"
#include "parser.h"
#include "tokenizer.h"
#include "debug.h"
#include "parser.h"

int main(int argc, char * argv[])
{
	if(argc < 2) {
		fprintf(stderr, "error: missing the file!\n");
		exit(EXIT_FAILURE);
	}

	if(atexit(drop_internal_allocators) != 0) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	init_symbols();

	struct stream stream;

	if(stream_init(&stream, argv[1]) == -1) {
		fprintf(stderr, "error: cannot initialize stream (%s: %s)\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct token * token_list = tokenize(&stream);

	struct ast * node;

	(void)parse(token_list, &node);

	if(!node) {
		fprintf(stderr, "error: there is no any rules to construct the fsm!\n");
		exit(EXIT_FAILURE);
	}

	debug_ast(stdout, node);

	stream_destroy(&stream);

	return 0;
}
