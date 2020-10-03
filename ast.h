#ifndef FSMCONV_AST_H
#define FSMCONV_AST_H 1

enum {
	AST_STATE,
	AST_CHARACTER,
	AST_SPECIAL_CHARACTER,
	AST_RULE,

	AST_RULE_LIST,
	AST_STATE_LIST,
	AST_CHARACTER_LIST,
};

struct ast_list
{
	struct ast_list * next;
	struct ast * node;
};

struct ast
{
	union {
		struct symbol * symbol;
		struct ast_list * list; /* for lists nodes */

		int ch;

		struct
		{
			struct symbol * key;
			struct ast * value;
		} special_character;

		struct
		{
			struct ast * source;
			struct ast * target;
			struct ast * by;
		} rule;
	} value;
	unsigned int type;
};


#endif /* FSMCONV_AST_H */
