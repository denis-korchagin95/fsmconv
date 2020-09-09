#ifndef NFA2DFACONV_PARSER_H
#define NFA2DFACONV_PARSER_H 1

#include <stdio.h>
#include "nfa_types.h"

extern struct token eof_token;

void init_parser(void);
struct nfa * parse(FILE * source); 

#endif /* NFA2DFACONV_PARSER_H */
