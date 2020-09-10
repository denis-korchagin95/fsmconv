#ifndef NFA2DFACONV_PARSER_TYPES_H
#define NFA2DFACONV_PARSER_TYPES_H 1

#define IDENTIFIER_MAX_SIZE 256

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

enum {
	SYMBOL_KEYWORD,
	SYMBOL_CHARACTER,
	SYMBOL_CHARACTER_LIST,
	SYMBOL_STATE,
	SYMBOL_TRANSITION,
	SYMBOL_RULE,
};

enum {
	KEYWORD_START,
	KEYWORD_END,
	KEYWORD_TO,
	KEYWORD_BY,
};

struct symbol
{
	union
	{
		struct symbol_transition
		{
			struct symbol * from_state;
			struct symbol * to_state;
		} transition;

		struct symbol_rule
		{
			struct symbol * transition;
			struct symbol * character_list;
		} rule;

		int code;
	} content;
	struct symbol * next;
	struct identifier * identifier;
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
