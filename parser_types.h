#ifndef NFA2DFACONV_PARSER_TYPES_H
#define NFA2DFACONV_PARSER_TYPES_H 1

#include "nfa_types.h"

#define IDENTIFIER_MAX_SIZE 256

enum {
	SYMBOL_KEYWORD,
	SYMBOL_CHARACTER_SET,
	SYMBOL_NFA_STATE,
};

enum {
	KEYWORD_START,
	KEYWORD_END,
	KEYWORD_TO,
	KEYWORD_BY,
};

struct symbol
{
	struct identifier * identifier;
	struct symbol * next;
	union
	{
		struct nfa_char_set * char_set;
		struct nfa_state * nfa_state;
		int code;
	} content;
	int type;
};

struct identifier
{
	char name[IDENTIFIER_MAX_SIZE];
	struct symbol * symbols;
	struct symbol ** last_symbol;
	struct identifier * next;
};

struct token
{
    int type;
    /* TODO: location in the source */
    union {
        int code;
	struct identifier * identifier;
    } content;
};

#endif /* NFA2DFACONV_PARSER_TYPES_H */
