#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include "util.h"
#include "fsm_types.h"
#include "visualize.h"
#include "parser.h"
#include "fsm_compiler.h"
#include "fsm.h"
#include "internal_allocators.h"

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

    FILE * input = fopen(input_file, "r");
    if (input == NULL) {
	    fprintf(stderr, "%s: %s\n", input_file, strerror(errno));
	    exit(1);
    }

    FILE * output;

    if (output_file == NULL)
	    output = stdout;
    else
    {
    	output = fopen(output_file, "w");
    	if (output == NULL) {
	    	fprintf(stderr, "%s: %s\n", output_file, strerror(errno));
	    	exit(1);
    	}
    }

    init_parser();

    struct symbol * parse_tree = parse(input);

    if (parse_tree == NULL) {
        printf("parse file error!\n");
	    fclose(input);
	    fclose(output);
        exit(1);
    }

    struct fsm * nfa = fsm_compile(parse_tree);

    if (print_input_fsm_only) {
        print_fsm(output, nfa, fsm_output_format, false);

        /* TODO: free(parse_tree); */
        /* TODO: free(nfa); */

	    fclose(input);
	    fclose(output);
        exit(0);
    }

    struct fsm * dfa = nfa_to_dfa(nfa);

    print_fsm(output, dfa, fsm_output_format, true);

    /* TODO: free(parse_tree); */
    /* TODO: free(nfa); */
    /* TODO: free(dfa); */

	fclose(input);
	fclose(output);

    exit(0);
}
