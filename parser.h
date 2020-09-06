#ifndef NFA2DFACONV_PARSER_H
#define NFA2DFACONV_PARSER_H 1

#include <stdio.h>

struct nfa;

extern struct nfa * parse_file(FILE * file);

#endif /* NFA2DFACONV_PARSER_H */
