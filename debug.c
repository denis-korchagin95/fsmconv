#include <stdio.h>

#include "parser.h"
#include "parser_types.h"
#include "debug.h"

static void print_tree_borders(FILE * output, int depth)
{
	if (depth <= 0) return;
	int i;
	for(i = 0; i < depth; ++i)
		fprintf(output, "\t|");
	fprintf(output, "\n");
	fprintf(output, "\t+-> ");
}


void debug_token(FILE * output, struct token * token)
{
	if(token == NULL || token == &eof_token)
		return;
	switch(token->type)
	{
		case TOKEN_INVALID:
			fprintf(output, "[INVALID %c]", token->content.code);
			break;
		case TOKEN_IDENTIFIER:
			fprintf(output, "[IDENTIFIER %s]", token->content.identifier->name);
			break;
		case TOKEN_CHARACTER:
			fprintf(output, "[CHARACTER %c]", token->content.code);
			break;
		case TOKEN_PUNCTUATOR:
			switch(token->content.code)
			{
				case PUNCTUATOR_SEMICOLON:
					fprintf(output, "[PUNCTUATOR ;]");
					break;
				case PUNCTUATOR_AT:
					fprintf(output, "[PUNCTUATOR @]");
					break;
				case PUNCTUATOR_COMMA:
					fprintf(output, "[PUNCTUATOR ,]");
					break;
				case PUNCTUATOR_HYPHEN_LESS:
					fprintf(output, "[PUNCTUATOR ->]");
					break;
				default:
					fprintf(output, "[PUNCTUATOR <unknown>]");
			}
			break;
		default:
			fprintf(output, "<unknown token>");
	}
}
	

void debug_symbol(FILE * output, struct symbol * symbol, int depth)
{
	if (symbol == NULL)
		return;
	int i;
	switch(symbol->type)
	{
		case SYMBOL_KEYWORD:
			fprintf(output, "{KEYWORD %s}", symbol->identifier->name);
			break;
		case SYMBOL_STATE:
			print_tree_borders(output, depth);
			fprintf(output, "{STATE %s}", symbol->identifier->name);
			break;
		case SYMBOL_CHARACTER_LIST:
			fprintf(output, "{CHARACTER_LIST #todo printing...}");
			break;
		case SYMBOL_TRANSITION:
			fprintf(output, "{TRANSITION}\n");
			debug_symbol(output, symbol->content.transition.from_state, depth + 1);
			fprintf(output, "\n");
			debug_symbol(output, symbol->content.transition.to_state, depth + 1);
			break;
		case SYMBOL_RULE:
			fprintf(output, "{RULE #todo printing...}");
			break;
		default:
			fprintf(output, "<unknown symbol>");
	}
}

