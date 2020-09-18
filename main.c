#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#include "parser.h"
#include "fsm_compiler.h"
#include "visualize.h"
#include "fsm_types.h"
#include "fsm.h"
#include "util.h"

enum {
    FSM_OUTPUT_FORMAT_NATIVE = 0,
    FSM_OUTPUT_FORMAT_DOT = 1,
};

static void usage(const char * program, FILE * output)
{
    fprintf(output, "Usage: file [options]\n\n");
    fprintf(output, "OPTIONS\n");
    fprintf(output, "\t--print-only\n");
    fprintf(output, "\t\tPrint only the given FSM and exit.\n");
    fprintf(output, "\t--format=[native|dot] (native by default)\n");
    fprintf(output, "\t\tPrint the FSM in a given format, where format can be one of 'native', or 'dot'.\n");
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

    int i;
    for(i = 2; i < argc; ++i) {
        const char * arg = argv[i];

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

    init_parser();

    FILE * file = fopen(input_file, "r");

    struct symbol * parse_tree = parse(file);

    fclose(file);

    if (parse_tree == NULL) {
        printf("parse file error!\n");
        exit(1);
    }

    struct fsm * nfa = fsm_compile(parse_tree);

    if (print_input_fsm_only) {
        print_fsm(stdout, nfa, fsm_output_format, false);

        /* TODO: free(parse_tree); */
        /* TODO: free(nfa); */

        exit(0);
    }

    struct fsm * dfa = nfa_to_dfa(nfa);

    print_fsm(stdout, dfa, fsm_output_format, true);

    /* TODO: free(parse_tree); */
    /* TODO: free(nfa); */
    /* TODO: free(dfa); */

    exit(0);
}
