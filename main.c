#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "internal_allocators.h"
#include "visualize.h"
#include "parser.h"
#include "stream.h"
#include "tokenizer.h"
#include "fsm_compiler.h"
#include "fsm.h"

enum {
    FSM_OUTPUT_FORMAT_NATIVE = 0,
    FSM_OUTPUT_FORMAT_DOT = 1,
};

static void usage(const char * program, FILE * output)
{
    const char * basename = program;
    const char * search;

    if ((search = strrchr(program, DIRECTORY_SEPARATOR)) != NULL)
        basename = search + 1;

    fprintf(output, "Usage: %s file [options]\n", basename);
    fprintf(output, "\n");
    fprintf(output, "DESCRIPTION\n");
    fprintf(output, "\tA command-line tool for converting FSA from one to another representation.\n");
    fprintf(output, "\n");
    fprintf(output, "OPTIONS\n");
    fprintf(output, "\t--print-only\n");
    fprintf(output, "\t\tPrint only the given FSM and exit.\n");
    fprintf(output, "\t--format=[native|dot] (native by default)\n");
    fprintf(output, "\t\tPrint the FSM in a given format, where format can be one of 'native', or 'dot'.\n");
    fprintf(output, "\t--output=<file>\n");
    fprintf(output, "\t\tPlace the output into <file>.\n");
    fflush(output);
}

static void print_fsm(FILE * output, struct fsm * fsm, int format, bool is_dfa)
{
    switch (format)
    {
        case FSM_OUTPUT_FORMAT_NATIVE:
            if (is_dfa)
                generate_dfa_language(output, fsm);
            else
                generate_nfa_language(output, fsm);
            break;
        case FSM_OUTPUT_FORMAT_DOT:
            if (is_dfa)
                visualize_dfa(output, fsm);
            else
                visualize_nfa(output, fsm);
            break;
    }
}

static bool print_input_fsm_only = false;
static int fsm_output_format = FSM_OUTPUT_FORMAT_NATIVE;

int main(int argc, char * argv[])
{
    if (argc < 2) {
        usage(argv[0], stdout);
        exit(0);
    }
    const char * input_file = argv[1];
    const char * output_file = NULL;

	atexit(drop_internal_allocators);

    int i;
    for(i = 2; i < argc; ++i) {
        const char * arg = argv[i];

	    if (!memcmp(arg, "--output=", 9)) {
		    output_file = arg + 9;
		    continue;
	    }

        if (!strcmp(arg, "--format=native")) {
            continue;
        }

        if (!strcmp(arg, "--format=dot")) {
            fsm_output_format = FSM_OUTPUT_FORMAT_DOT;
            continue;
        }

        if (!strcmp(arg, "--print-only")) {
            print_input_fsm_only = true;
            continue;
        }

        fprintf(stderr, "unknown option: %s\n", arg);
        exit(0);
    }

    init_symbols();

	struct stream stream;

	if(stream_init(&stream, input_file) == -1) {
		fprintf(stderr, "error: cannot initialize stream (%s: %s)\n", input_file, strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct token * token_list = tokenize(&stream);

	struct ast * tree = NULL;

   	(void)parse(token_list, &tree);

    if (tree == NULL) {
        fprintf(stderr, "error: there is no any rules to construct the fsm!\n");
		stream_destroy(&stream);
        exit(EXIT_FAILURE);
    }

	struct fsm * nfa = fsm_compile(tree);

    FILE * output = NULL;

    if (output_file != NULL) {
		output = fopen(output_file, "w");
		if(!output)
			fprintf(stderr, "warning: failed to open the output file '%s': %s!\n", output_file, strerror(errno));
	}

	if(!output)
		output = stdout;

	if(print_input_fsm_only) {
		print_fsm(output, nfa, fsm_output_format, false);
		stream_destroy(&stream);
		exit(EXIT_FAILURE);
	}

	struct fsm * dfa = nfa_to_dfa(nfa);

	print_fsm(output, dfa, fsm_output_format, true);

	stream_destroy(&stream);
    exit(EXIT_SUCCESS);

	return 0;
}
