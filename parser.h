#ifndef NFA2DFACONV_PARSER_H
#define NFA2DFACONV_PARSER_H 1

#include <stdio.h>
#include "nfa_types.h"

extern struct token eof_token;

enum {
	TOKEN_INVALID,
	TOKEN_IDENTIFIER,
	TOKEN_CHARACTER,
	TOKEN_PUNCTUATOR,
};

enum {
	PUNCTUATOR_HYPHEN_LESS, /* '->' */
	PUNCTUATOR_SEMICOLON,   /* ';' */
	PUNCTUATOR_AT,          /* '@' */
	PUNCTUATOR_COMMA,	/* ',' */
};

struct nfa * parse(FILE * source); 

#endif /* NFA2DFACONV_PARSER_H */
