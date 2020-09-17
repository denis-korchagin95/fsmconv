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

int main(int argc, char * argv[])
{
  init_parser();

  if (argc < 2) {
      fprintf(stderr, "%s: missing operand\n", argv[0]);
      exit(1);
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
