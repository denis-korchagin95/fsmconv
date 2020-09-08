#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"
#include "parser_types.h"
#include "allocator.h"
#include "debug.h"

#define PUTBACK_BUFFER_SIZE 3
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
static int putback_buffer_pos;
static struct identifier * identifiers[IDENTIFIER_TABLE_SIZE];

struct token eof_token = { 0 };

static bool is_keyword(struct identifier * identifier)
{
	struct symbol * it = identifier->symbols;
	while(it != NULL) {
		if (it->type == SYMBOL_KEYWORD)
			return true;
		it = it->next;
	}
	return false;
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

static struct token * read_token(void)
{
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
		symbol->identifier = identifier;
		symbol->content.code = keyword->code;

		(*identifier->last_symbol) = symbol;
		identifier->last_symbol = &symbol->next;
	}
}

struct nfa * parse(FILE * file)
{
	set_source(file);

	struct token * token;

	while(1) {
		token = read_token();
		if (token == &eof_token)
			break;
		debug_token(stdout, token);
		printf("\n");
	}
	
	return NULL;
}
