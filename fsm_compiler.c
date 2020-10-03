#include "fsm_compiler.h"
#include "ast.h"
#include "fsm_state.h"
#include "internal_allocators.h"
#include "fsm_transition.h"
#include "fsm.h"
#include "symbol.h"
#include "tokenizer.h"
#include "parser.h"
#include "fsm_state_list.h"

static void fsm_compile_transition(struct fsm_state * source, struct fsm_state * target, struct ast * node)
{
	int ch;

	if(node->type == AST_SPECIAL_CHARACTER)
		ch = node->value.special_character.value->value.ch;
	else
		ch = node->value.ch;

	struct fsm_transition * transition = fsm_state_search_transition_by_character(source, ch);

	if(!transition)
	{
		transition = alloc_fsm_transition();
		transition->states = NULL;
		transition->next = source->transitions;
		transition->ch = ch;

		source->transitions = transition;
	}

	if(!transition->states || !fsm_transition_has_state(transition, target->id)) {
		struct fsm_state_list * state_list = alloc_fsm_state_list();
		state_list->state_id = target->id;
		state_list->next = transition->states;

		transition->states = state_list;
	}
}

static struct fsm_state * fsm_compile_state(struct fsm * fsm, struct ast * node)
{
	struct symbol * symbol = node->value.symbol;

	struct fsm_state * state = fsm_search_state_by_id(fsm, symbol->value.state.state_id);

	if(state)
		return state;

	unsigned int attrs = 0;

	if(symbol->attributes & SYMBOL_ATTRIBUTE_INITIAL_STATE)
		attrs |= FSM_STATE_ATTR_INITIAL;
	if(symbol->attributes & SYMBOL_ATTRIBUTE_FINAL_STATE)
		attrs |= FSM_STATE_ATTR_FINAL;

	state = alloc_fsm_state();
	state->next = NULL;
	state->fsm = fsm;
	state->transitions = NULL;
	state->subset = NULL;
	state->name = symbol->identifier->name;
	state->id = symbol->value.state.state_id;
	state->attrs = attrs;

	*fsm->last_state = state;
	fsm->last_state = &state->next;

	return state;
}

static void fsm_compile_rule(struct fsm * fsm, struct ast * node)
{
	struct fsm_state * source = fsm_compile_state(fsm, node->value.rule.source);
	struct fsm_state * target = fsm_compile_state(fsm, node->value.rule.target);

	struct ast * by = node->value.rule.by;

	if(by->type == AST_CHARACTER || (by->type == AST_SPECIAL_CHARACTER && by->value.special_character.value->type == AST_CHARACTER)) {
		fsm_compile_transition(source, target, by);
		return;
	}

	struct ast_list * it = by->type == AST_SPECIAL_CHARACTER ? by->value.special_character.value->value.list : by->value.list;

	while(it) {
		fsm_compile_transition(source, target, it->node);
		it = it->next;
	}
}

struct fsm * fsm_compile(struct ast * node)
{
	struct fsm * fsm = alloc_fsm();
	fsm->type = FSM_TYPE_UNDEFINED;
	fsm->states = NULL;
	fsm->last_state = &fsm->states;
	fsm->state_count = parser_last_state_id();

	/* compile rules */
	struct ast_list * it = node->value.list;

	while(it) {
		fsm_compile_rule(fsm, it->node);
		it = it->next;
	}

	fsm->type = fsm_determine_type(fsm);

	return fsm;
}
