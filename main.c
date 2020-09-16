#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#include "parser.h"
#include "nfa_compiler.h"
#include "visualize.h"
#include "nfa_types.h"
#include "nfa.h"

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

  struct nfa * nfa = nfa_compile(parse_tree);

  generate_nfa_language(stdout, nfa);

  exit(0);
}
