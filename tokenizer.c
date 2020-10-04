#include "tokenizer.h"
#include "internal_allocators.h"
#include "symbol.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define IDENTIFIER_TABLE_SIZE   (101)
#define IDENTIFIER_MAX_SIZE     (256)

struct token eof_token = { {0}, {0}, &eof_token, TOKEN_EOF };

static struct identifier * identifiers[IDENTIFIER_TABLE_SIZE];

static unsigned int get_string_hash(const char * name)
{
	unsigned int hash = 0;
	while(*name) {
		hash = hash * 31 + (unsigned int) *name;
		++name;
	}
	return hash;
}

static struct identifier * identifier_search(unsigned int hash, const char * name)
{
	unsigned int index = hash % IDENTIFIER_TABLE_SIZE;
	struct identifier * it;
	for(it = identifiers[index]; it; it = it->next) {
		if (strcmp(name, it->name) == 0) {
			return it;
		}
	}
	return NULL;
}

static struct identifier * identifier_insert(unsigned int hash, const char * name, unsigned int len)
{
	unsigned int index = hash % IDENTIFIER_TABLE_SIZE;

	struct identifier * identifier = alloc_identifier();

	identifier->name = (char *) alloc_bytes(len + 1);
	strncpy(identifier->name, name, len);
	identifier->name[len] = '\0';

	identifier->symbols = NULL;

	identifier->next = identifiers[index];
	identifiers[index] = identifier;
	return identifier;
}

static void read_character(struct stream * stream, struct token * token, int ch)
{
	if (ch == EOF) {
		fprintf(stderr, "error: not terminated character constant!\n");
		exit(EXIT_FAILURE);
	}
	else if(ch == '\'') {
		fprintf(stderr, "error: empty character constant!\n");
		exit(EXIT_FAILURE);
	}
	else if(ch == '\n') {
		fprintf(stderr, "error: new line character in cannot allow in character constant!\n");
		exit(EXIT_FAILURE);
	}
	else if(ch == '\\') { /* handle escape sequence */
		int next_ch = stream_getchar(stream);
		if (next_ch == '\'') {
			ch = '\'';
		}
		else if(next_ch == '\\') {
			ch = '\\';
		}
		else {
			fprintf(stderr, "error: unknown escape sequence in character constant: \\%c\n", next_ch);
			exit(EXIT_FAILURE);
		}
	}

	token->type = TOKEN_CHARACTER;
	token->value.code = ch;
	ch = stream_getchar(stream);

	if (ch != '\'') {
		fprintf(stderr, "error: not terminated character constant!\n");
		exit(EXIT_FAILURE);
	}
}

static void read_identifier(struct stream * stream, struct token * token, int ch)
{
	static char buffer[IDENTIFIER_MAX_SIZE];
	int i = 0;
	unsigned int hash = 0;

	while(ch != EOF && (isalpha(ch) || isdigit(ch) || ch == '_') && i < IDENTIFIER_MAX_SIZE - 1) {
		buffer[i++] = ch;
		hash = ch + 31 * hash;
		ch = stream_getchar(stream);
	}
	buffer[i] = '\0';
	if (ch != EOF)
		stream_ungetchar(stream, ch);
	if(i >= IDENTIFIER_MAX_SIZE - 1) {
		fprintf(stderr, "warning: identifier too long!\n");
		while(isalpha(ch) || isdigit(ch) || ch == '_')
			ch = stream_getchar(stream);
		stream_ungetchar(stream, ch);
	}

	struct identifier * identifier = identifier_search(hash, buffer);
	if (! identifier)
		identifier = identifier_insert(hash, buffer, i);

	token->value.identifier = identifier;
}

static void read_token(struct stream * stream, struct token * token, int ch)
{
	if (isalpha(ch) || ch == '_') {
		token->type = TOKEN_IDENTIFIER;
		read_identifier(stream, token, ch);

		struct symbol * symbol = lookup_symbol(token->value.identifier, SYMBOL_KEYWORD);
		if(symbol) {
			token->type = TOKEN_KEYWORD;
			token->value.code = symbol->value.code;
		}

		return;
	}

	switch(ch) {
		case ',':
			token->type = TOKEN_PUNCTUATOR;
			token->value.code = PUNCTUATOR_COMMA;
			return;
		case ';':
			token->type = TOKEN_PUNCTUATOR;
			token->value.code = PUNCTUATOR_SEMICOLON;
			return;
		case '#':
			{
				int next_ch = stream_getchar(stream);
				if(isalpha(next_ch) || next_ch == '_') {
					token->type = TOKEN_DIRECTIVE;
					read_identifier(stream, token, next_ch);
					return;
				}
				stream_ungetchar(stream, next_ch);
			}
			goto invalid;
		case '@':
			{
				int next_ch = stream_getchar(stream);
				if(isalpha(next_ch) || next_ch == '_') {
					token->type = TOKEN_SPECIAL_CHARACTER;
					read_identifier(stream, token, next_ch);
					return;
				}
				stream_ungetchar(stream, next_ch);
			}
			goto invalid;
		case '-':
			{
				int next_ch = stream_getchar(stream);
				if (next_ch == '>') {
					token->type = TOKEN_PUNCTUATOR;
					token->value.code = PUNCTUATOR_HYPHEN_LESS;
					return;
				}
				stream_ungetchar(stream, next_ch);
			}
			break;
		case '\'':
			read_character(stream, token, stream_getchar(stream));
			return;
	}

invalid:
	token->type = TOKEN_INVALID;
	token->value.code = ch;
}

struct identifier * identifier_create(const char * name)
{
	unsigned int hash = get_string_hash(name);
	unsigned int len = strlen(name);
	return identifier_insert(hash, name, len);
}

struct token * tokenize(struct stream * stream)
{
	struct token * token_list, * token, ** last_token;

	token_list = &eof_token;
	last_token = &token_list;

	int ch = stream_getchar(stream);

	struct source_location location;

	while(ch != EOF) {
		if(! isspace(ch)) {
			location = stream->location;

			if(ch == '/') { /* try to skip comment */
				int next_ch = stream_getchar(stream);
				if(next_ch == '/') {
					do
						ch = stream_getchar(stream);
					while(ch != '\n' && ch != EOF);
					goto next_char;
				}
				stream_ungetchar(stream, next_ch);
			}

			token = alloc_token();
			token->location = location;

			read_token(stream, token, ch);

			token->next = &eof_token;
			*last_token = token;
			last_token = &token->next;
		}
next_char:
		ch = stream_getchar(stream);
	}

	return token_list;
}
