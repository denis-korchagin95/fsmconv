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

static const char * version = "0.0.1";

static void usage(const char * program, FILE * output)
{
    const char * basename = strrchr(program, DIRECTORY_SEPARATOR);
    if (basename != NULL)
        ++basename;
    fprintf(output, "%s version %s\n", basename, version);
    fprintf(output, "Usage: %s source\n", basename);
	fflush(output);
}

int main(int argc, char * argv[])
{
  init_parser();

  if (argc < 2) {
      usage(argv[0], stdout);
      exit(0);
  }

  FILE * file = fopen(argv[1], "r");

  struct symbol * parse_tree = parse(file);

  fclose(file);

  if (parse_tree == NULL) {
	  printf("parse file error!\n");
	  exit(1);
  }

  struct fsm * nfa = fsm_compile(parse_tree);

  /* TODO: free(parse_tree); */
  /* TODO: free(nfa); */

  struct fsm * dfa = nfa_to_dfa(nfa);

  generate_dfa_language(stdout, dfa);

  /* TODO: free(dfa); */

  exit(0);
}
