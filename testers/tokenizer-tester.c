#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "tokenizer.h"
#include "print.h"
#include "allocator.h"
#include "symbol.h"
#include "parser.h"

int main(int argc, char * argv[])
{
	if(argc < 2) {
		fprintf(stderr, "error: missing the file!\n");
		exit(EXIT_FAILURE);
	}

	if (atexit(area_cleanup) != 0) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	init_symbols();

	struct stream stream;

	if (stream_init(&stream, argv[1]) == -1) {
		fprintf(stderr, "error: cannot initialize stream (%s: %s)\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}


	struct token * token_list = tokenize(&stream);

	struct token * token = token_list;

	while(token != &eof_token) {
		print_token(stdout, token);
		puts("");
		token = token->next;
	}

	stream_destroy(&stream);

	return 0;
}
