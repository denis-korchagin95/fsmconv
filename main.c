#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#include "util.h"
#include "parser.h"
#include "nfa_types.h"
#include "parser_types.h"
#include "debug.h"
#include "allocator.h"
#include "nfa_compiler.h"
#include "visualize.h"
#include "nfa.h"


static const char * nfa_graph_filename = "./nfa.dot";
static const char * dfa_graph_filename = "./dfa.dot";


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

  struct nfa * nfa;

  nfa = nfa_compile(parse_tree);

  FILE * output = fopen(nfa_graph_filename, "w");
  if(!output) {
    fprintf(stderr, "file '%s': %s\n", nfa_graph_filename, strerror(errno));
    exit(1);
  }

  visualize_nfa(nfa, output);

  /*
  struct nfa_state * it;
  list_foreach(it, nfa->states) {
      struct nfa_state_list * empty_closure = nfa_state_empty_closure(it);
      fprintf(stdout, "state(%s) has empty closure: ",it->name);
      debug_nfa_state_list(stdout, empty_closure);
      fprintf(stdout, "\n");
  }
  */

  fclose(output);

  /*
  debug_symbol(stdout, parse_tree, 0);
  puts("");
  */

  /* show_allocation_stats(stdout); */

  exit(0);

  return 0;
}
