#ifndef FSMCONV_TOKENIZER_H
#define FSMCONV_TOKENIZER_H 1

#include "stream.h"

struct source_location;
struct symbol;

enum {
	TOKEN_EOF,
	TOKEN_INVALID,
	TOKEN_IDENTIFIER,
	TOKEN_KEYWORD,
	TOKEN_DIRECTIVE,
	TOKEN_CHARACTER,
	TOKEN_SPECIAL_CHARACTER,
	TOKEN_PUNCTUATOR,
};

enum {
	KEYWORD_TO,
	KEYWORD_BY,
};

enum {
	DIRECTIVE_INITIAL,
	DIRECTIVE_FINAL,
};

enum {
	PUNCTUATOR_HYPHEN_LESS, /* '->' */
	PUNCTUATOR_SEMICOLON,   /* ';' */
	PUNCTUATOR_COMMA,       /* ',' */
};

struct identifier
{
	char * name;
	struct symbol * symbols;
	struct identifier * next;
};

struct token
{
	struct source_location location;
    union {
        int code;
        struct identifier * identifier;
    } value;
	struct token * next;
    int type;
};

extern struct token eof_token;

extern struct token * tokenize(struct stream * stream);
extern struct identifier * identifier_create(const char * name);

#endif /* FSMCONV_TOKENIZER_H */
