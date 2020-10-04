#ifndef FSMCONV_SYMBOL_H
#define FSMCONV_SYMBOL_H 1

enum {
	SYMBOL_STATE,
	SYMBOL_SPECIAL_CHARACTER,
	SYMBOL_KEYWORD,
	SYMBOL_DIRECTIVE,
};

#define SYMBOL_ATTRIBUTE_INITIAL_STATE  (1<<0)
#define SYMBOL_ATTRIBUTE_FINAL_STATE    (1<<1)
#define SYMBOL_ATTRIBUTE_USED           (1<<2)

#define attach_symbol(identifier, symbol)       \
	{                                           \
		(symbol)->next = (identifier)->symbols; \
		(identifier)->symbols = (symbol);       \
	}

struct ast;
struct identifier;

struct symbol
{
	struct identifier * identifier;
	struct symbol * next;
	union {
		struct
		{
			unsigned int state_id;
			struct ast * node;
		} state;

		struct
		{
			unsigned int code;
			struct ast * node;
		} directive;

		struct ast * node; /* used for special characters */
		unsigned int code; /* used for keywords */
	} value;
	unsigned int attributes:6;
	unsigned int type:2;
	unsigned int reserved:20;
};

extern struct symbol * lookup_symbol(struct identifier * identifier, int type);

#endif /* FSMCONV_SYMBOL_H */
