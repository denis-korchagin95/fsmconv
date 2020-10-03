#include "debug.h"
#include "parser.h"
#include "tokenizer.h"
#include "symbol.h"
#include "ast.h"
#include "util.h"
#include "fsm_state_list.h"

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_TREE_DEPTH		(1)
#define MAX_TREE_DEPTH_BITS	(sizeof(*is_tree_node_latest_in_depth) * MAX_TREE_DEPTH * CHAR_BIT)

static unsigned char is_tree_node_latest_in_depth[MAX_TREE_DEPTH];

static void do_ident(FILE * output, int depth)
{
	if(depth <= 0) return;
	int i;
	for(i = 0; i < depth - 1; ++i) {
		bool is_node_latest = u8_bits_check(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, i);
		fprintf(output, "%s%s", is_node_latest ? " " : "|", i + 1 == depth ? "" : "\t");
	}
	fprintf(output, "|\n");
	for(i = 0; i < depth - 1; ++i) {
		bool is_node_latest = u8_bits_check(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, i);
		fprintf(output, "%s%s", i + 1 == depth ? "" : is_node_latest ? " " : "|", i + 1 == depth ? "" : "\t");
	}
	fprintf(output, "+-> ");
}

void debug_token(FILE * output, struct token * token)
{
	if(! token || token == &eof_token)
		return;
	switch(token->type)
	{
		case TOKEN_EOF:
			fprintf(output, "[EOF]");
			break;
		case TOKEN_INVALID:
			fprintf(output, "[INVALID %c]", token->value.code);
			break;
		case TOKEN_SPECIAL_CHARACTER:
			fprintf(output, "[SPECIAL_CHARACTER %s]", token->value.identifier->name);
			break;
		case TOKEN_DIRECTIVE:
			fprintf(output, "[DIRECTIVE %s]", token->value.identifier->name);
			break;
		case TOKEN_IDENTIFIER:
			fprintf(output, "[IDENTIFIER %s]", token->value.identifier->name);
			break;
		case TOKEN_KEYWORD:
			fprintf(output, "[KEYWORD ");
			switch(token->value.code)
			{
				case KEYWORD_TO: fprintf(output, "to"); break;
				case KEYWORD_BY: fprintf(output, "by"); break;
				default:
					fprintf(output, "unknown");
			}
			fprintf(output, "]");
			break;
		case TOKEN_CHARACTER:
			fprintf(output, "[CHARACTER %c]", token->value.code);
			break;
		case TOKEN_PUNCTUATOR:
			switch(token->value.code)
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
	
void debug_fsm_state_list(FILE * output, struct fsm_state_list * list)
{
    fprintf(output, "{");
    while(list) {
        fprintf(output, " %u%s", list->state_id, !list->next ? " " : ",");
        list = list->next;
    }
    fprintf(output, "}");
}

static void do_debug_ast(FILE * output, struct ast * ast, int depth)
{
	if(depth >= MAX_TREE_DEPTH_BITS) {
		fprintf(stderr, "error: the tree is too many depths!\n");
		exit(EXIT_FAILURE);
	}
	if(! ast) {
		fprintf(output, "<bad ast node>");
		return;
	}
	do_ident(output, depth);
	switch(ast->type)
	{
		case AST_RULE_LIST:
			{
				struct ast_list * it = ast->value.list;
				unsigned int count = 0;
				while(it) {
					++count;
					it = it->next;
				}
				fprintf(output, "RuleList<%u>\n", count);
				it = ast->value.list;
				while(it) {
					if(! it->next)
						u8_bits_set(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, depth);
					else
						u8_bits_unset(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, depth);
					do_debug_ast(output, it->node, depth + 1);
					it = it->next;
				}
			}
			break;
		case AST_RULE:
			fprintf(output, "Rule<>\n");
			u8_bits_unset(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, depth);
			do_debug_ast(output, ast->value.rule.source, depth + 1);
			u8_bits_unset(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, depth);
			do_debug_ast(output, ast->value.rule.target, depth + 1);
			if(ast->value.rule.by->type == AST_CHARACTER_LIST)
				u8_bits_set(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, depth);
			else
				u8_bits_unset(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, depth);
			do_debug_ast(output, ast->value.rule.by, depth + 1);
			break;
		case AST_CHARACTER_LIST:
			{
				struct ast_list * it = ast->value.list;
				unsigned int count = 0;
				while(it) {
					++count;
					it = it->next;
				}
				fprintf(output, "CharacterList<%u>\n", count);
				it = ast->value.list;
				while(it) {
					do_debug_ast(output, it->node, depth + 1);
					it = it->next;
				}
			}
			break;
		case AST_STATE_LIST:
			{
				struct ast_list * it = ast->value.list;
				while(it) {
					do_debug_ast(output, it->node, depth + 1);
					it = it->next;
				}
			}
			break;
		case AST_SPECIAL_CHARACTER:
			{
				fprintf(output, "SpecialCharacter<%s>\n", ast->value.special_character.key->identifier->name);
			}
			break;
		case AST_CHARACTER:
			{
				fprintf(output, "Character<%c>\n", ast->value.ch);
			}
			break;
		case AST_STATE:
			{
				struct symbol * symbol = ast->value.symbol;
				fprintf(output, "State<%d> { name: \"%s\", attributes: ( ", symbol->value.state.state_id, symbol->identifier->name);
				if(symbol->attributes & SYMBOL_ATTRIBUTE_INITIAL_STATE)
					fprintf(output, "initial ");
				if(symbol->attributes & SYMBOL_ATTRIBUTE_FINAL_STATE)
					fprintf(output, "final ");
				fprintf(output, ") }\n");
			}
			break;
		default:
			fprintf(output, "<unknown ast node>\n");
	}
}

void debug_ast(FILE * output, struct ast * ast)
{
	u8_bits_set(is_tree_node_latest_in_depth, MAX_TREE_DEPTH_BITS, 0);
	do_debug_ast(output, ast, 0);
	fflush(output);
}
