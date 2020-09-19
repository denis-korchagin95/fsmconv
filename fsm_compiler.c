#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "character_list.h"
#include "fsm_types.h"
#include "parser_types.h"
#include "parser.h"
#include "allocator.h"
#include "util.h"
#include "fsm.h"
#include "fsm_state.h"

static struct fsm_state * fsm_state_compile(struct fsm * fsm, struct symbol * symbol)
{
	struct fsm_state * state;

	state = fsm_search_state_by_id(fsm, symbol->content.state.id);

	if (state != NULL)
		return state;

	state = ___alloc_fsm_state();
	state->next = NULL;
	state->fsm = fsm;
	state->transitions = NULL;
	state->subset = NULL;
	state->name = symbol->content.state.identifier->name;
	state->id = symbol->content.state.id;
	state->attrs = 0;

	(*fsm->last_state) = state;
	fsm->last_state = &state->next;

	return state;
}

static void fsm_transition_compile(struct fsm_state * source, struct fsm_state * target, struct symbol * character)
{
	struct fsm_transition * transition;
	struct fsm_state_list * state_list;
	int ch;

	if (character->type == SYMBOL_SPECIAL_CHARACTER_BUILTIN)
	    ch = character->content.special_character.value->content.code;
	else
	    ch = character->content.code;

	transition = fsm_state_search_transition_by_character(source, ch);

	if (transition == NULL)
	{
		transition = ___alloc_fsm_transition();
		transition->states = NULL;
		transition->next = source->transitions;
		transition->ch = ch;

		source->transitions = transition;
	}

	if (! fsm_transition_has_state(transition, target)) {
		state_list = ___alloc_fsm_state_list();
		state_list->state_id = target->id;
		state_list->next = transition->states;

		transition->states = state_list;
	}
}

static void fsm_rule_compile(struct fsm * fsm, struct symbol * symbol)
{
	struct fsm_state * source, * target;
	struct symbol * character, * transition;

	transition = symbol->content.rule.transition;

	source = fsm_state_compile(fsm, transition->content.transition.source);
	target = fsm_state_compile(fsm, transition->content.transition.target);

	character = symbol->content.rule.character_list;

	if (character->type == SYMBOL_CHARACTER || character->type == SYMBOL_SPECIAL_CHARACTER_BUILTIN) {
		fsm_transition_compile(source, target, character);
		return;
	}

	character = character->next;

	while(character != NULL) {
		fsm_transition_compile(source, target, character);
		character = character->next;
	}
}

static void fsm_directive_start_compile(struct fsm * fsm, struct symbol * directive)
{
	struct symbol * state;
	struct fsm_state * fsm_state;

	if (directive->content.symbol->type == SYMBOL_STATE) {
		fsm_state = fsm_search_state_by_id(fsm, directive->content.symbol->content.state.id);
		if (fsm_state != NULL)
			fsm_state->attrs |= FSM_STATE_ATTR_INITIAL;
		return;
	}

	state = directive->content.symbol->next;

	while(state != NULL) {
		fsm_state = fsm_search_state_by_id(fsm, state->content.state.id);
		if (fsm_state != NULL)
			fsm_state->attrs |= FSM_STATE_ATTR_INITIAL;
		state = state->next;
	}
}

static void fsm_directive_end_compile(struct fsm * fsm, struct symbol * directive)
{
	struct symbol * state;
	struct fsm_state * fsm_state;

	if (directive->content.symbol->type == SYMBOL_STATE) {
		fsm_state = fsm_search_state_by_id(fsm, directive->content.symbol->content.state.id);
		if (fsm_state != NULL)
			fsm_state->attrs |= FSM_STATE_ATTR_FINISHED;
		return;
	}

	state = directive->content.symbol->next;

	while(state != NULL) {
		fsm_state = fsm_search_state_by_id(fsm, state->content.state.id);
		if (fsm_state != NULL)
			fsm_state->attrs |= FSM_STATE_ATTR_FINISHED;
		state = state->next;
	}
}

struct fsm * fsm_compile(struct symbol * symbol)
{
	struct fsm * fsm;
	struct symbol * statement;

	fsm = ___alloc_fsm();
    fsm->type = FSM_TYPE_UNDEFINED;
	fsm->states = NULL;
	fsm->last_state = &fsm->states;
	fsm->state_count = parser_last_state_id();

	if (symbol->type == SYMBOL_STATEMENT && symbol->content.symbol->type == SYMBOL_RULE) {
		fsm_rule_compile(fsm, symbol->content.symbol);
		return fsm;
	}

	statement = symbol->next;

	/* compile rules */
	while(statement != NULL) {
		if (statement->content.symbol->type == SYMBOL_RULE)
		    fsm_rule_compile(fsm, statement->content.symbol);
        statement = statement->next;
	}

	/* determination type of the FSM */
	fsm->type = fsm_determine_type(fsm);

	statement = symbol->next;

	/* compile directives */
	while(statement != NULL) {
		switch(statement->content.symbol->type)
		{
			case SYMBOL_DIRECTIVE_START:
				fsm_directive_start_compile(fsm, statement->content.symbol);
				break;
			case SYMBOL_DIRECTIVE_END:
				fsm_directive_end_compile(fsm, statement->content.symbol);
				break;
		}
		statement = statement->next;
	}

	return fsm;
}
