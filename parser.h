#ifndef NFA2DFACONV_PARSER_H
#define NFA2DFACONV_PARSER_H 1

#include <stdio.h>
#include "nfa_types.h"

enum {
	TOKEN_EOF = -1,
	TOKEN_INVALID = 0,

	TOKEN_IDENTIFIER,
	TOKEN_CHARACTER,
	TOKEN_PUNCTUATOR,
};

enum {
	PUNCTUATOR_HYPHEN_LESS, /* '->' */
	PUNCTUATOR_SEMICOLON,   /* ';' */
	PUNCTUATOR_AT,          /* '@' */
};

extern struct nfa * parse_file(FILE * file);

#endif /* NFA2DFACONV_PARSER_H */
