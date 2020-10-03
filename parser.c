#include "parser.h"
#include "tokenizer.h"
#include "symbol.h"
#include "internal_allocators.h"
#include "ast.h"
#include "fsm.h"
#include "fsm_transition.h"

#include <stdlib.h>
#include <stdbool.h>

struct builtin_symbol
{
	char * name;
	int type;
	int code;
};

static struct builtin_symbol builtin_symbols[] = {
	{ "to",    		SYMBOL_KEYWORD, 	KEYWORD_TO    		},
	{ "by",    		SYMBOL_KEYWORD, 	KEYWORD_BY   		},
	{ "initial",	SYMBOL_DIRECTIVE, 	DIRECTIVE_INITIAL	},
	{ "final",		SYMBOL_DIRECTIVE, 	DIRECTIVE_FINAL 	},
};

void init_symbols(void)
{
	int i = 0, len = sizeof(builtin_symbols) / sizeof(*builtin_symbols);

	struct symbol * symbol;
	struct identifier * identifier;

	for(; i < len; ++i) {
		struct builtin_symbol * builtin_symbol = builtin_symbols + i;

		identifier = identifier_create(builtin_symbol->name);

		symbol = alloc_symbol();
		symbol->identifier = identifier;
		symbol->next = NULL;
		symbol->attributes = SYMBOL_ATTRIBUTE_USED;
		symbol->type = builtin_symbol->type;
		symbol->reserved = 0;


		if(builtin_symbol->type == SYMBOL_DIRECTIVE)
		{
			symbol->value.directive.code = builtin_symbol->code;
			symbol->value.directive.node = NULL;
		}
		else
			symbol->value.code = builtin_symbol->code;

		attach_symbol(identifier, symbol);
	}


	/* @epsilon - special character */
	struct ast * value = alloc_ast();
	value->type = AST_CHARACTER;
	value->value.ch = EPSILON_CHAR;

	struct ast * node = alloc_ast();
	node->type = AST_SPECIAL_CHARACTER;
	node->value.special_character.value = value;

	identifier = identifier_create("epsilon");

	symbol = alloc_symbol();
	symbol->identifier = identifier;
	symbol->next = NULL;
	symbol->attributes = SYMBOL_ATTRIBUTE_USED;
	symbol->type = SYMBOL_SPECIAL_CHARACTER;
	symbol->reserved = 0;
	symbol->value.node = node;

	node->value.special_character.key = symbol;

	attach_symbol(identifier, symbol);
}

static unsigned int state_id = 0;

static const char * token_stringify(struct token * token)
{
	switch(token->type)
	{
		case TOKEN_EOF:		   	return "eof";
		case TOKEN_INVALID:    	return "invalid";
		case TOKEN_KEYWORD:	   	return "keyword";
		case TOKEN_DIRECTIVE:	return "directive";
		case TOKEN_IDENTIFIER: 	return "identifier";
		case TOKEN_PUNCTUATOR: 	return "punctuator";
		case TOKEN_CHARACTER:  	return "character";
	}
	return "<unknown token>";
}


void init_parser(void) { } 


struct token * parse_state(struct token * token, struct ast ** state)
{
	if(token->type != TOKEN_IDENTIFIER) {
		fprintf(stderr, "error: expected identifier but given %s\n", token_stringify(token));
		exit(EXIT_FAILURE);
	}
	struct symbol * symbol = lookup_symbol(token->value.identifier, SYMBOL_STATE);

	if(!symbol)
	{
		struct ast * node = alloc_ast();
		node->type = AST_STATE;

		symbol = alloc_symbol();
		symbol->identifier = token->value.identifier;
		symbol->next = NULL;
		symbol->value.state.state_id = state_id++;
		symbol->value.state.node = node;
		symbol->attributes = 0;
		symbol->type = SYMBOL_STATE;
		symbol->reserved = 0;

		node->value.symbol = symbol;

		attach_symbol(token->value.identifier, symbol);
	}

	*state = symbol->value.state.node;

	return token->next;
}

struct token * parse_character(struct token * token, struct ast ** node)
{
	if(token->type != TOKEN_CHARACTER && token->type != TOKEN_SPECIAL_CHARACTER) {
		fprintf(stderr, "error: expected character constant or special character but given %s\n", token_stringify(token));
		exit(EXIT_FAILURE);
	}

	if(token->type == TOKEN_CHARACTER)
	{
		struct ast * character = alloc_ast();
		character->type = AST_CHARACTER;
		character->value.ch = token->value.code;
		*node = character;
		return token->next;
	}

	struct symbol * symbol = lookup_symbol(token->value.identifier, SYMBOL_SPECIAL_CHARACTER);

	if(!symbol) {
		fprintf(stderr, "error: undefined special character @%s\n", token->value.identifier->name);
		exit(EXIT_FAILURE);
	}

	*node = symbol->value.node;

	return token->next;
}

struct token * parse_character_list(struct token * token, void ** node, bool * is_list)
{
	struct ast * character;

	token = parse_character(token, &character);

	if(!(token->type == TOKEN_PUNCTUATOR && token->value.code == PUNCTUATOR_COMMA)) {
		*node = (void *) character;
		*is_list = false;
		return token;
	}

	struct ast_list * list, ** last_element, * new_element;

	list = NULL;
	last_element = &list;

	new_element = alloc_ast_list();
	new_element->node = character;
	new_element->next = NULL;

	*last_element = new_element;
	last_element = &new_element->next;

	do
	{
		token = parse_character(token->next, &character);

		new_element = alloc_ast_list();
		new_element->node = character;
		new_element->next = NULL;

		*last_element = new_element;
		last_element = &new_element->next;
	}
	while(token->type == TOKEN_PUNCTUATOR && token->value.code == PUNCTUATOR_COMMA);

	*node = (void *) list;
	*is_list = true;

	return token;
}

struct token * parse_rule(struct token * token, struct ast ** rule)
{
	struct ast * source, * target;

	token = parse_state(token, &source);

	if(!((token->type == TOKEN_KEYWORD && token->value.code == KEYWORD_TO)
		|| (token->type == TOKEN_PUNCTUATOR && token->value.code == PUNCTUATOR_HYPHEN_LESS))) {
		fprintf(stderr, "error: expected keyword 'to' or '->' punctuator that must refer to other state but given %s!\n", token_stringify(token));
		exit(EXIT_FAILURE);
	}

	token = parse_state(token->next, &target);

	if(!(token->type == TOKEN_KEYWORD && token->value.code == KEYWORD_BY)) {
		fprintf(stderr, "error: expected keyword 'by' but given %s\n", token_stringify(token));
		exit(EXIT_FAILURE);
	}

	void * object;
	bool is_list;
	struct ast * by;

	token = parse_character_list(token->next, &object, &is_list);

	if(is_list) {
		by = alloc_ast();
		by->type = AST_CHARACTER_LIST;
		by->value.list = (struct ast_list *) object;
	}
	else
		by = (struct ast *) object;

	struct ast * node = alloc_ast();
	node->type = AST_RULE;
	node->value.rule.source = source;
	node->value.rule.target = target;
	node->value.rule.by = by;
	*rule = node;

	return token;
}

struct token * parse_state_list(struct token * token, void ** node, bool * is_list)
{
	struct ast * state;

	token = parse_state(token, &state);

	if(!(token->type == TOKEN_PUNCTUATOR && token->value.code == PUNCTUATOR_COMMA)) {
		*node = (void *) state;
		*is_list = false;
		return token;
	}

	struct ast_list * list, ** last_element, * new_element;

	list = NULL;
	last_element = &list;

	new_element = alloc_ast_list();
	new_element->node = state;
	new_element->next = NULL;

	*last_element = new_element;
	last_element = &new_element->next;

	do
	{
		token = parse_state(token->next, &state);

		new_element = alloc_ast_list();
		new_element->node = state;
		new_element->next = NULL;

		*last_element = new_element;
		last_element = &new_element->next;
	}
	while(token->type == TOKEN_PUNCTUATOR && token->value.code == PUNCTUATOR_COMMA);

	*node = (void *) list;
	*is_list = true;

	return token;
}

struct token * parse_directive_initial(struct token * token, struct symbol * symbol, struct ast ** directive)
{
	/*
		We does not need to create any tree-nodes,
		because this directive only changes the symbol
		who represents the 'initial' directive.
	*/
	*directive = NULL;

	void * object;
	bool is_list;

	token = parse_state_list(token->next, &object, &is_list);

	/*
		It needs to unset initial flag for all previous states and then mark new ones.
		Free memory if we got a list of states.
	*/
	if(symbol->value.directive.node) {
		fprintf(stderr, "warning: duplicate of initial directive!\n");

		if(symbol->value.directive.node->type == AST_STATE) {
			symbol->value.directive.node->value.symbol->attributes &= ~SYMBOL_ATTRIBUTE_INITIAL_STATE;
			symbol->value.directive.node = NULL;
		}
		else {
			struct ast_list ** next, ** current = &symbol->value.directive.node->value.list;
			while(*current) {
				(*current)->node->value.symbol->attributes &= ~SYMBOL_ATTRIBUTE_INITIAL_STATE;
				next = &(*current)->next;
				free_ast_list(*current);
				*current = NULL;
				current = next;
			}
		}
	}

	struct ast * node;

	if(is_list) {
		struct ast_list * it = (struct ast_list *) object;

		node = alloc_ast();
		node->type = AST_STATE_LIST;
		node->value.list = it;

		while(it) {
			it->node->value.symbol->attributes |= SYMBOL_ATTRIBUTE_INITIAL_STATE;
			it = it->next;
		} 
	}
	else {
		node = (struct ast *) object;
		node->value.symbol->attributes |= SYMBOL_ATTRIBUTE_INITIAL_STATE;
	}

	symbol->value.directive.node = node;

	return token;
}

struct token * parse_directive_final(struct token * token, struct symbol * symbol, struct ast ** directive)
{
	/*
		We does not need to create any tree-nodes,
		because this directive only changes the symbol
		who represents the 'final' directive.
	*/
	*directive = NULL;

	void * object;
	bool is_list;

	token = parse_state_list(token->next, &object, &is_list);

	/*
		It needs to unset final flag for all previous states and then mark new ones.
		Free memory if we got a list of states
	*/
	if(symbol->value.directive.node) {
		fprintf(stderr, "warning: duplicate of final directive!\n");

		if(symbol->value.directive.node->type == AST_STATE) {
			symbol->value.directive.node->value.symbol->attributes &= ~SYMBOL_ATTRIBUTE_FINAL_STATE;
			symbol->value.directive.node = NULL;
		}
		else {
			struct ast_list ** next, ** current = &symbol->value.directive.node->value.list;
			while(*current) {
				(*current)->node->value.symbol->attributes &= ~SYMBOL_ATTRIBUTE_FINAL_STATE;
				next = &(*current)->next;
				free_ast_list(*current);
				*current = NULL;
				current = next;
			}
		}
	}

	struct ast * node;

	if(is_list) {
		struct ast_list * it = (struct ast_list *) object;

		node = alloc_ast();
		node->type = AST_STATE_LIST;
		node->value.list = it;

		while(it) {
			it->node->value.symbol->attributes |= SYMBOL_ATTRIBUTE_FINAL_STATE;
			it = it->next;
		} 
	}
	else {
		node = (struct ast *) object;
		node->value.symbol->attributes |= SYMBOL_ATTRIBUTE_FINAL_STATE;
	}

	symbol->value.directive.node = node;

	return token;
}

struct token * parse_directive(struct token * token, struct symbol * symbol, struct ast ** directive)
{
	switch(symbol->value.directive.code)
	{
		case DIRECTIVE_INITIAL:
			token = parse_directive_initial(token, symbol, directive);
			break;
		case DIRECTIVE_FINAL:
			token = parse_directive_final(token, symbol, directive);
			break;
		default:
			fprintf(stderr, "error: unknown directive!\n");
			exit(EXIT_FAILURE);
	}
	return token;
}

struct token * parse_statement(struct token * token, struct ast ** statement)
{
	switch(token->type)
	{
		case TOKEN_DIRECTIVE:
			{
				struct symbol * symbol = lookup_symbol(token->value.identifier, SYMBOL_DIRECTIVE);
				if(!symbol) {
					fprintf(stderr, "error: undefined directive #%s!\n", token->value.identifier->name);
					exit(EXIT_FAILURE);
				}
				token = parse_directive(token, symbol, statement);
			}
			break;
		case TOKEN_IDENTIFIER:
			token = parse_rule(token, statement);
			break;
		default:
			fprintf(stderr, "error: expected directive or identifier but given %s\n", token_stringify(token));
			exit(EXIT_FAILURE);
	}

	if(!(token->type == TOKEN_PUNCTUATOR && token->value.code == PUNCTUATOR_SEMICOLON)) {
		fprintf(stderr, "error: expected ';' but given %s\n", token_stringify(token));
		exit(EXIT_FAILURE);
	}

	return token->next;
}

struct token * parse(struct token * token, struct ast ** tree)
{
	struct ast_list * list, ** last_element, * new_element;
	struct ast * statement;

	list = NULL;
	last_element = &list;

	do
	{
		token = parse_statement(token, &statement);

		if(statement) {
			new_element = alloc_ast_list();
			new_element->next = NULL;
			new_element->node = statement;

			*last_element = new_element;
			last_element = &new_element->next;
		}
	}
	while(token != &eof_token);

	if(!list) {
		*tree = NULL;
		return token;
	}

	struct ast * node = alloc_ast();
	node->type = AST_RULE_LIST;
	node->value.list = list;

	*tree = node;

	return token;
}

unsigned int parser_last_state_id(void)
{
	return state_id;
}
