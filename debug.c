#include <stdio.h>
#include <stdbool.h>

#include "parser.h"
#include "parser_types.h"
#include "debug.h"

static void print_tree_borders(FILE * output, int depth, bool is_last)
{
	if (depth <= 0) return;
	int i;
	for(i = 0; i < depth; i++) {
		if (is_last && i != depth - 1)
			fprintf(output, "\t");
		else
			fprintf(output, "\t|");
	}
	fprintf(output, "\n");
	for(i = 0; i < depth; i++) {
		if(is_last)
			fprintf(output, "\t");
		else
			fprintf(output, "\t%s", i + 1 == depth ? "" : "|");
	}
	fprintf(output, "+->  ");
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
	

void debug_symbol(FILE * output, struct symbol * symbol, int depth, bool is_last)
{
	if (symbol == NULL)
		return;
	switch(symbol->type)
	{
		case SYMBOL_KEYWORD:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{KEYWORD %s}", symbol->identifier->name);
			break;
		case SYMBOL_CHARACTER:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{CHARACTER %c}", symbol->content.code);
			break;
		case SYMBOL_STATE:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{STATE %s}", symbol->identifier->name);
			break;
		case SYMBOL_DIRECTIVE_END:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{DIRECTIVE_END}\n");
			debug_symbol(output, symbol->content.symbol, depth + 1, is_last);
			break;
		case SYMBOL_DIRECTIVE_START:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{DIRECTIVE_START}\n");
			debug_symbol(output, symbol->content.symbol, depth + 1, is_last);
			break;
		case SYMBOL_STATE_LIST:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{STATE_LIST}\n");
			{
				struct symbol * it = symbol->next;
				while(it != NULL) {
					debug_symbol(output, it, depth + 1, is_last);
					if(it->next != NULL)
						puts("");
					it = it->next;
				}
			}
			break;
		case SYMBOL_CHARACTER_LIST:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{CHARACTER_LIST}\n");
			{
				struct symbol * it = symbol->next;
				while(it != NULL) {
					debug_symbol(output, it, depth + 1, is_last);
					if (it->next != NULL)
						puts("");
					it = it->next;
				}
			}
			break;
		case SYMBOL_TRANSITION:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{TRANSITION}\n");
			debug_symbol(output, symbol->content.transition.from_state, depth + 1, false);
			fprintf(output, "\n");
			debug_symbol(output, symbol->content.transition.to_state, depth + 1, false);
			break;
		case SYMBOL_RULE:
			print_tree_borders(output, depth, is_last);
			fprintf(output, "{RULE}\n");
			debug_symbol(output, symbol->content.rule.transition, depth + 1, false);
			fprintf(output, "\n");
			debug_symbol(output, symbol->content.rule.character_list, depth + 1, true);
			break;
		default:
			fprintf(output, "<unknown symbol>");
	}
}

