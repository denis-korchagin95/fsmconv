#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "nfa_types.h"
#include "parser.h"
#include "parser_types.h"
#include "allocator.h"
#include "debug.h"

#define PUTBACK_BUFFER_SIZE 3
#define PUTBACK_TOKEN_BUFFER_SIZE 3
#define IDENTIFIER_TABLE_SIZE 101

struct keyword
{
	char * name;
	int code;
};

static struct keyword keywords[] = {
	{ "start", KEYWORD_START },
	{ "end",   KEYWORD_END   },
	{ "to",    KEYWORD_TO    },
	{ "by",    KEYWORD_BY    },
};

static FILE * source;

static int putback_buffer[PUTBACK_BUFFER_SIZE];
static int putback_buffer_pos = 0;
static struct token * putback_token_buffer[PUTBACK_TOKEN_BUFFER_SIZE];
static int putback_token_buffer_pos = 0;
static struct identifier * identifiers[IDENTIFIER_TABLE_SIZE];

struct token eof_token = { 0 };

static void identifier_attach_symbol(struct identifier * identifier, struct symbol * symbol)
{
	if (symbol->type == SYMBOL_STATE)
		symbol->content.identifier = identifier;
	(*identifier->last_symbol) = symbol;
	identifier->last_symbol = &symbol->next;
}

static struct symbol * search_symbol(struct identifier * identifier, int type)
{
	struct symbol * it = identifier->symbols;
	while(it != NULL) {
		if (it->type == type) {
			return it;
		}
		it = it->next;
	}
	return NULL;
}

static const char * keyword_to_string(int type)
{
	switch(type)
	{
		case KEYWORD_START:	return "start";
		case KEYWORD_END: 	return "end";
		case KEYWORD_TO: 	return "to";
		case KEYWORD_BY: 	return "by";
	}
	return "<unknown token>";
}

static const char * token_type(struct token * token)
{
	switch(token->type)
	{
		case TOKEN_INVALID:    return "invalid";
		case TOKEN_IDENTIFIER: return "identifier";
		case TOKEN_PUNCTUATOR: return "punctuator";
		case TOKEN_CHARACTER:  return "character";
	}
	return "<unknown token>";
}

static bool is_punctuator_as(struct token * token, int type)
{
	return token->type == TOKEN_PUNCTUATOR && token->content.code == type;
}

static bool is_keyword(struct token * token)
{
	return token->type == TOKEN_IDENTIFIER && search_symbol(token->content.identifier, SYMBOL_KEYWORD) != NULL;
}

static bool is_keyword_as(struct token * token, int type)
{
	if (token->type != TOKEN_IDENTIFIER)
		return false;
	struct symbol * symbol = search_symbol(token->content.identifier, SYMBOL_KEYWORD);
	if (symbol == NULL)
		return false;
	return symbol->content.code == type;
}

static uint32_t hash(const char * name)
{
	uint32_t value = 0;
	while(*name != '\0') {
		value = *name + 31 * value;
		++name;
	}
	return value;
}

static void set_source(FILE * file)
{
	source = file;
	putback_buffer_pos = 0;
}

static int getch(void)
{
	if(putback_buffer_pos > 0)
		return putback_buffer[--putback_buffer_pos];
	return fgetc(source);
}

static void ungetch(int ch)
{
	if (ch == EOF) return;
	if (putback_buffer_pos >= PUTBACK_BUFFER_SIZE)
		putback_buffer_pos = 0;
	putback_buffer[putback_buffer_pos++] = ch;
}

static struct identifier * identifier_search(uint32_t hash, const char * name)
{
	uint32_t index = hash % IDENTIFIER_TABLE_SIZE;
	struct identifier * it;
	for(it = identifiers[index]; it != NULL; it = it->next) {
		if (strcmp(name, it->name) == 0) {
			return it;
		}
	}
	return NULL;
}

static struct identifier * identifier_insert(uint32_t hash, const char * name)
{
	uint32_t index = hash % IDENTIFIER_TABLE_SIZE;

	struct identifier * identifier = ___alloc_identifier();
	strncpy(identifier->name, name, IDENTIFIER_MAX_SIZE);
	identifier->symbols = NULL;
	identifier->last_symbol = &identifier->symbols;

	identifier->next = identifiers[index];
	identifiers[index] = identifier;
	return identifier;
}

static struct token * read_character(int ch, struct token * token)
{
	if (ch == EOF) {
		fprintf(stderr, "error: not terminated character constant!\n");
		exit(1);
	}
	else if(ch == '\'') {
		fprintf(stderr, "error: empty character constant!\n");
		exit(1);
	}
	else if(ch == '\n') {
		fprintf(stderr, "error: new line character in cannot allow in character constant!\n");
		exit(1);
	}
	else if(ch == '\\') { /* handle escape sequence */
		int next_ch = getch();
		if (next_ch == '\'') {
			ch = '\'';
		}
		else if(next_ch == '\\') {
			ch = '\\';
		}
		else {
			fprintf(stderr, "error: unknown escape sequence in character constant: \\%c\n", next_ch);
			exit(1);
		}
	}

	token->type = TOKEN_CHARACTER;
	token->content.code = ch;
	ch = getch();

	if (ch != '\'') {
		fprintf(stderr, "error: not terminated character constant!\n");
		exit(1);
	}

	return token;
}

static struct token * read_identifier(int ch, struct token * token)
{
	static char buffer[IDENTIFIER_MAX_SIZE];
	int i = 0;
	uint32_t hash = 0;

	while(ch != EOF && (isalpha(ch) || isdigit(ch) || ch == '_') && i < IDENTIFIER_MAX_SIZE - 1) {
		buffer[i++] = ch;
		hash = ch + 31 * hash;
		ch = getch();
	}
	buffer[i] = '\0';
	if (ch != EOF)
		ungetch(ch);
	if(i >= IDENTIFIER_MAX_SIZE - 1) {
		fprintf(stderr, "warning: identifier too long!\n");
		while(isalpha(ch) || isdigit(ch) || ch == '_')
			ch = getch();
		ungetch(ch);
	}

	struct identifier * identifier = identifier_search(hash, buffer);
	if (identifier == NULL)
		identifier = identifier_insert(hash, buffer);

	token->type = TOKEN_IDENTIFIER;
	token->content.identifier = identifier;
	return token;
}

static void unread_token(struct token * token)
{
	if (token == &eof_token) return;
	if (putback_token_buffer_pos >= PUTBACK_TOKEN_BUFFER_SIZE)
		putback_token_buffer_pos = 0;
	putback_token_buffer[putback_token_buffer_pos++] = token;
}

static struct token * read_token(void)
{
	if(putback_token_buffer_pos > 0)
		return putback_token_buffer[--putback_token_buffer_pos];

	int ch;
	struct token * token;

repeat:
	ch = getch();

	while(isspace(ch))
		ch = getch();

	if (ch == EOF)
		return &eof_token;

	if (ch == '#') { /* single-line comment */
		do
			ch = getch();
		while(ch != '\n' && ch != EOF);
		goto repeat;
	}

	token = ___alloc_token();

	if (isalpha(ch) || ch == '_')
		return read_identifier(ch, token);

	switch(ch) {
		case ',':
			token->type = TOKEN_PUNCTUATOR;
			token->content.code = PUNCTUATOR_COMMA;
			goto ret;
		case ';':
			token->type = TOKEN_PUNCTUATOR;
			token->content.code = PUNCTUATOR_SEMICOLON;
			goto ret;
		case '@':
			token->type = TOKEN_PUNCTUATOR;
			token->content.code = PUNCTUATOR_AT;
			goto ret;
		case '-':
			{
				int next_ch = getch();
				if (next_ch == '>') {
					token->type = TOKEN_PUNCTUATOR;
					token->content.code = PUNCTUATOR_HYPHEN_LESS;
					goto ret;
				}
				ungetch(next_ch);
			}
			break;
		case '\'':
			return read_character(getch(), token);
	}

	token->type = TOKEN_INVALID;
	token->content.code = ch;

ret:
	return token;
}

void init_parser(void)
{
	struct symbol * symbol;
	struct identifier * identifier;
	struct keyword * keyword;
	int keyword_count, i;

	keyword_count = sizeof(keywords) / sizeof(keywords[0]);

	for(i = 0; i < keyword_count; ++i) {
		keyword = keywords + i;

		identifier = identifier_insert(hash(keyword->name), keyword->name);

		symbol = ___alloc_symbol();
		symbol->type = SYMBOL_KEYWORD;
		symbol->content.code = keyword->code;
		symbol->next = NULL;

		identifier_attach_symbol(identifier, symbol);
	}
}

struct symbol * parse_state(void)
{
	struct token * token = read_token();
	if (token->type != TOKEN_IDENTIFIER) {
		fprintf(stderr, "error: expected identifier but given %s\n", token_type(token));
		exit(1);
	}
	if (is_keyword(token)) {
		fprintf(stderr, "error: keyword '%s' can't be used for naming nfa state\n", token->content.identifier->name);
		exit(1);
	}
	struct symbol * state = search_symbol(token->content.identifier, SYMBOL_STATE);
	if (state != NULL)
		return state;

	state = ___alloc_symbol();
	state->type = SYMBOL_STATE;
	state->next = NULL;

	identifier_attach_symbol(token->content.identifier, state);

	return state;
}

struct symbol * parse_transition(void)
{
	struct symbol * from_state = parse_state();
	struct token * token = read_token();
	if (!is_keyword_as(token, KEYWORD_TO) && !is_punctuator_as(token, PUNCTUATOR_HYPHEN_LESS)) {
		fprintf(stdout, "error: expected keyword 'to' or '->' punctuator that must refer to other state but given %s!\n", token_type(token));
		exit(1);
	}
	struct symbol * to_state = parse_state();

	struct symbol * transition = ___alloc_symbol();
	transition->type = SYMBOL_TRANSITION;
	transition->next = NULL;
	transition->content.transition.from_state = from_state;
	transition->content.transition.to_state = to_state;

	return transition;
}

struct symbol * parse_character(void)
{
	struct token * token = read_token();
	if (token->type != TOKEN_CHARACTER) {
		fprintf(stdout, "error: expected character constant but given %s!\n", token_type(token));
		exit(1);
	}
	struct symbol * character = ___alloc_symbol();
	character->next = NULL;
	character->type = SYMBOL_CHARACTER;
	character->content.code = token->content.code;

	return character;
}

struct symbol * parse_character_list(void)
{
	struct symbol * character_list, * character, ** last_character;
	struct token * token;

	character_list = ___alloc_symbol();
	character_list->next = NULL;
	character_list->type = SYMBOL_CHARACTER_LIST;
	last_character = &character_list->next;

	character = parse_character();
	(*last_character) = character;
	last_character = &character->next;

	token = read_token();
	while(is_punctuator_as(token, PUNCTUATOR_COMMA)) {
		character = parse_character();
		(*last_character) = character;
		last_character = &character->next;
		token = read_token();
	}
	unread_token(token);

	return character_list;
}

struct symbol * parse_rule(void)
{
	struct symbol * transition, * character_list, * rule;
	struct token * token;

	transition = parse_transition();
	token = read_token();
	if (!is_keyword_as(token, KEYWORD_BY)) {
		fprintf(stderr, "error: expected keyword 'by' but given %s\n", token_type(token));
		exit(1);
	}
	character_list = parse_character_list();
	token = read_token();
	if(!is_punctuator_as(token, PUNCTUATOR_SEMICOLON)) {
		fprintf(stderr, "error: expected ';' but given %s\n", token_type(token));
		exit(1);
	}

	rule = ___alloc_symbol();
	rule->next = NULL;
	rule->type = SYMBOL_RULE;
	rule->content.rule.transition = transition;
	rule->content.rule.character_list = character_list;

	return rule;
}

struct symbol * parse_state_list(void)
{
	struct symbol * state, * state_list, ** last_state;
	struct token * token;

	state_list = ___alloc_symbol();
	state_list->type = SYMBOL_STATE_LIST;
	state_list->next = NULL;

	last_state = &state_list->next;

	state = parse_state();

	(*last_state) = state;
	last_state = &state->next;

	token = read_token();
	while(is_punctuator_as(token, PUNCTUATOR_COMMA)) {
		state = parse_state();
		(*last_state) = state;
		last_state = &state->next;
		token = read_token();
	}
	unread_token(token);

	return state_list;
}

struct symbol * parse_start_directive(void)
{
	struct symbol * directive;
	struct token * token;

	directive = ___alloc_symbol();
	directive->type = SYMBOL_DIRECTIVE_START;
	directive->next = NULL;
	directive->content.symbol = parse_state_list();

	token = read_token();
	if(!is_punctuator_as(token, PUNCTUATOR_SEMICOLON)) {
		fprintf(stdout, "error: expected ';' but given %s\n", token_type(token));
		exit(1);
	}

	return directive;
}

struct symbol * parse_end_directive(void)
{
	struct symbol * directive;
	struct token * token;

	directive = ___alloc_symbol();
	directive->type = SYMBOL_DIRECTIVE_END;
	directive->next = NULL;
	directive->content.symbol = parse_state_list();

	token = read_token();
	if(!is_punctuator_as(token, PUNCTUATOR_SEMICOLON)) {
		fprintf(stdout, "error: expected ';' but given %s\n", token_type(token));
		exit(1);
	}

	return directive;
}

struct symbol * parse_statement(void)
{
	struct symbol * symbol, * statement;
	struct token * token;

	token = read_token();
	if (token->type != TOKEN_IDENTIFIER) {
		fprintf(stdout, "error: expected directive keyword or identifier but given %s\n", token_type(token));
		exit(1);
	}

	statement = ___alloc_symbol();
	statement->type = SYMBOL_STATEMENT;
	statement->next = NULL;

	symbol = search_symbol(token->content.identifier, SYMBOL_KEYWORD);
	if (symbol == NULL) {
		unread_token(token);
		statement->content.symbol = parse_rule();
		return statement;
	}
	if (symbol->content.code == KEYWORD_START) {
		statement->content.symbol = parse_start_directive();
		return statement;
	}
	else if(symbol->content.code == KEYWORD_END) {
		statement->content.symbol = parse_end_directive();
		return statement;
	}
	fprintf(stdout, "error: keyword '%s' cannot start any directive!\n", keyword_to_string(symbol->content.code));
	exit(1);
}

struct symbol * parse(FILE * file)
{
	struct symbol * statement, * statement_list, ** last_statement;
	struct token * token;

	set_source(file);

	statement = parse_statement();
	token = read_token();
	if (token == &eof_token)
		return statement;

	statement_list = ___alloc_symbol();
	statement_list->type = SYMBOL_STATEMENT_LIST;
	statement_list->next = statement;

	last_statement = &statement->next;

	unread_token(token);
	while(1) {
		statement = parse_statement();

		(*last_statement) = statement;
		last_statement = &statement->next;

		token = read_token();
		if (token == &eof_token)
			break;
		unread_token(token);
	}

	return statement_list;
}
