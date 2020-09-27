#include "debug.h"
#include "parser.h"

static void print_tree_borders(FILE * output, int depth)
{
	if (depth <= 0) return;
	int i;
	for(i = 0; i < depth; i++) {
		fprintf(output, "\t|");
	}
	fprintf(output, "\n");
	for(i = 0; i < depth; i++) {
		fprintf(output, "\t%s", i == depth - 1 ? "" : "|");
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
		case TOKEN_SPECIAL_CHARACTER:
			fprintf(output, "[SPECIAL_CHARACTER %s]", token->content.identifier->name);
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
	switch(symbol->type)
	{
		case SYMBOL_SPECIAL_CHARACTER_BUILTIN:
			print_tree_borders(output, depth);
			fprintf(output, "{SPECIAL_CHARACTER_BUILTIN %s}\n", symbol->content.special_character.identifier->name);
			debug_symbol(output, symbol->content.special_character.value, depth + 1);
			break;
		case SYMBOL_SPECIAL_CHARACTER_USER_DEFINED:
			print_tree_borders(output, depth);
			fprintf(output, "{SPECIAL_CHARACTER_USER_DEFINED %s}\n", symbol->content.special_character.identifier->name);
			debug_symbol(output, symbol->content.special_character.value, depth + 1);
			break;
		case SYMBOL_STATEMENT_LIST:
			print_tree_borders(output, depth);
			fprintf(output, "{STATEMENT_LIST}\n");
			{
				struct symbol * it = symbol->next;
				while(it != NULL) {
					debug_symbol(output, it, depth + 1);
					if(it->next != NULL)
						puts("");
					it = it->next;
				}
			}
			break;
		case SYMBOL_STATEMENT:
			print_tree_borders(output, depth);
			fprintf(output, "{STATEMENT}\n");
			debug_symbol(output, symbol->content.symbol, depth + 1);
			break;
		case SYMBOL_KEYWORD:
			print_tree_borders(output, depth);
			switch(symbol->content.code)
			{
				case KEYWORD_INITIAL:
					fprintf(output, "{KEYWORD initial}");
					break;
				case KEYWORD_FINAL:
					fprintf(output, "{KEYWORD final}");
					break;
				case KEYWORD_TO:
					fprintf(output, "{KEYWORD to}");
					break;
				case KEYWORD_BY:
					fprintf(output, "{KEYWORD by}");
					break;
				default:
					fprintf(output, "{KEYWORD <unknown>}");
			}
			break;
		case SYMBOL_CHARACTER:
			print_tree_borders(output, depth);
			fprintf(output, "{CHARACTER %c}", symbol->content.code);
			break;
		case SYMBOL_STATE:
			print_tree_borders(output, depth);
			fprintf(output, "{STATE %s}", symbol->content.state.identifier->name);
			break;
		case SYMBOL_DIRECTIVE_FINAL:
			print_tree_borders(output, depth);
			fprintf(output, "{DIRECTIVE_FINAL}\n");
			debug_symbol(output, symbol->content.symbol, depth + 1);
			break;
		case SYMBOL_DIRECTIVE_INITIAL:
			print_tree_borders(output, depth);
			fprintf(output, "{DIRECTIVE_INITIAL}\n");
			debug_symbol(output, symbol->content.symbol, depth + 1);
			break;
		case SYMBOL_STATE_LIST:
			print_tree_borders(output, depth);
			fprintf(output, "{STATE_LIST}\n");
			{
				struct symbol * it = symbol->next;
				while(it != NULL) {
					debug_symbol(output, it, depth + 1);
					if(it->next != NULL)
						puts("");
					it = it->next;
				}
			}
			break;
		case SYMBOL_CHARACTER_LIST:
			print_tree_borders(output, depth);
			fprintf(output, "{CHARACTER_LIST}\n");
			{
				struct symbol * it = symbol->next;
				while(it != NULL) {
					debug_symbol(output, it, depth + 1);
					if (it->next != NULL)
						puts("");
					it = it->next;
				}
			}
			break;
		case SYMBOL_TRANSITION:
			print_tree_borders(output, depth);
			fprintf(output, "{TRANSITION}\n");
			debug_symbol(output, symbol->content.transition.source, depth + 1);
			fprintf(output, "\n");
			debug_symbol(output, symbol->content.transition.target, depth + 1);
			break;
		case SYMBOL_RULE:
			print_tree_borders(output, depth);
			fprintf(output, "{RULE}\n");
			debug_symbol(output, symbol->content.rule.transition, depth + 1);
			fprintf(output, "\n");
			debug_symbol(output, symbol->content.rule.character_list, depth + 1);
			break;
		default:
			fprintf(output, "<unknown symbol>");
	}
}

void debug_fsm_state_list(FILE * output, struct fsm_state_list * list)
{
    fprintf(output, "{");
    while(list != NULL) {
        fprintf(output, " %u%s", list->state_id, list->next == NULL ? " " : ",");
        list = list->next;
    }
    fprintf(output, "}");
}
