#ifndef FSMCONV_PARSER_TYPES_H
#define FSMCONV_PARSER_TYPES_H 1

#define IDENTIFIER_MAX_SIZE 256

enum {
	TOKEN_INVALID,
	TOKEN_IDENTIFIER,
	TOKEN_CHARACTER,
	TOKEN_SPECIAL_CHARACTER,
	TOKEN_PUNCTUATOR,
};

enum {
	PUNCTUATOR_HYPHEN_LESS, /* '->' */
	PUNCTUATOR_SEMICOLON,   /* ';' */
	PUNCTUATOR_COMMA,       /* ',' */
};

enum {
	KEYWORD_START,
	KEYWORD_END,
	KEYWORD_TO,
	KEYWORD_BY,
};


enum {
	SYMBOL_KEYWORD,
	SYMBOL_CHARACTER,
	SYMBOL_SPECIAL_CHARACTER_BUILTIN,
	SYMBOL_SPECIAL_CHARACTER_USER_DEFINED,
	SYMBOL_CHARACTER_LIST,
	SYMBOL_STATE,
	SYMBOL_STATE_LIST,
	SYMBOL_TRANSITION,
	SYMBOL_RULE,
	SYMBOL_DIRECTIVE_START,
	SYMBOL_DIRECTIVE_END,
	SYMBOL_STATEMENT,
	SYMBOL_STATEMENT_LIST,
};

struct symbol
{
	union
	{
		struct
		{
			struct symbol * source;
			struct symbol * target;
		} transition;

		struct
		{
			struct symbol * transition;
			struct symbol * character_list;
		} rule;

		struct
		{
			struct identifier * identifier;
			struct symbol * value;
		} special_character;

		struct
		{
			struct identifier * identifier;
			uint32_t id;
		} state;

		struct symbol * symbol;

		struct identifier * identifier;

		int code;
	} content;
	struct symbol * next;
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

#endif /* FSMCONV_PARSER_TYPES_H */
