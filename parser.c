#include <ctype.h>

#include "parser.h"
#include "parser_types.h"
#include "allocator.h"
#include "debug.h"

#define PUTBACK_BUFFER_SIZE 3
#define IDENTIFIER_MAX_SIZE 256

static FILE * source;
static int putback_buffer[PUTBACK_BUFFER_SIZE];
static int putback_buffer_pos;

struct token eof_token = { 0 };

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
	int i = 0;

	while(ch != EOF && (isalpha(ch) || isdigit(ch) || ch == '_') && i < IDENTIFIER_MAX_SIZE) {
		token->content.identifier[i++] = ch;
		ch = getch();
	}
	if (ch != EOF)
		ungetch(ch);
	if(i >= IDENTIFIER_MAX_SIZE) {
		fprintf(stderr, "warning: identifier too long!\n");
		while(isalpha(ch) || isdigit(ch) || ch == '_')
			ch = getch();
		ungetch(ch);
	}

	token->type = TOKEN_IDENTIFIER;
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
