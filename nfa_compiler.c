#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "nfa_types.h"
#include "parser_types.h"
#include "allocator.h"
#include "util.h"
#include "nfa.h"
#include "nfa_state.h"

static struct nfa_state * nfa_state_compile(struct nfa * nfa, struct symbol * symbol)
{
	struct nfa_state * state;

	state = nfa_search_state_by_id(nfa, symbol->content.state.id);

	if (state != NULL)
		return state;

	state = ___alloc_nfa_state();
	state->next = NULL;
    state->owner = nfa;
    state->transitions = NULL;
    state->subset = NULL;
    state->name = symbol->content.state.identifier->name;
    state->id = symbol->content.state.id;
	state->attrs = 0;

	(*nfa->last_state) = state;
	nfa->last_state = &state->next;

	return state;
}

static void nfa_transition_compile(struct nfa_state * source, struct nfa_state * target, struct symbol * character)
{
	struct nfa_transition * transition;
	struct nfa_state_list * state_list;
	int ch;

	if (character->type == SYMBOL_SPECIAL_CHARACTER_BUILTIN)
	    ch = character->content.special_character.value->content.code;
	else
	    ch = character->content.code;

	transition = nfa_state_search_transition_by_character(source, ch);

	if (transition == NULL)
	{
		transition = ___alloc_nfa_transition();
		transition->states = NULL;
		transition->next = source->transitions;
		transition->ch = ch;

		source->transitions = transition;
	}

	if (! nfa_transition_has_state(transition, target)) {
		state_list = ___alloc_nfa_state_list();
		state_list->state_id = target->id;
		state_list->next = transition->states;

		transition->states = state_list;
	}
}

static void nfa_rule_compile(struct nfa * nfa, struct symbol * symbol)
{
	struct nfa_state * source, * target;
	struct symbol * character, * transition;

	transition = symbol->content.rule.transition;

	source = nfa_state_compile(nfa, transition->content.transition.source);
	target = nfa_state_compile(nfa, transition->content.transition.target);

	character = symbol->content.rule.character_list;

	if (character->type == SYMBOL_CHARACTER || character->type == SYMBOL_SPECIAL_CHARACTER_BUILTIN) {
		nfa_transition_compile(source, target, character);
		return;
	}

	character = character->next;

	while(character != NULL) {
		nfa_transition_compile(source, target, character);
		character = character->next;
	}
}

static void nfa_directive_start_compile(struct nfa * nfa, struct symbol * directive)
{
	struct symbol * state;
	struct nfa_state * nfa_state;

	if (directive->content.symbol->type == SYMBOL_STATE) {
		nfa_state = nfa_search_state_by_id(nfa, directive->content.symbol->content.state.id);
		if (nfa_state != NULL)
			nfa_state->attrs |= NFA_STATE_ATTR_INITIAL;
		return;
	}

	state = directive->content.symbol->next;

	while(state != NULL) {
		nfa_state = nfa_search_state_by_id(nfa, state->content.state.id);
		if (nfa_state != NULL)
			nfa_state->attrs |= NFA_STATE_ATTR_INITIAL;
		state = state->next;
	}
}

static void nfa_directive_end_compile(struct nfa * nfa, struct symbol * directive)
{
	struct symbol * state;
	struct nfa_state * nfa_state;

	if (directive->content.symbol->type == SYMBOL_STATE) {
		nfa_state = nfa_search_state_by_id(nfa, directive->content.symbol->content.state.id);
		if (nfa_state != NULL)
			nfa_state->attrs |= NFA_STATE_ATTR_FINISHED;
		return;
	}

	state = directive->content.symbol->next;

	while(state != NULL) {
		nfa_state = nfa_search_state_by_id(nfa, state->content.state.id);
		if (nfa_state != NULL)
			nfa_state->attrs |= NFA_STATE_ATTR_FINISHED;
		state = state->next;
	}
}

struct nfa * nfa_compile(struct symbol * symbol)
{
	struct nfa * nfa;
	struct symbol * statement;

	nfa = ___alloc_nfa();
	nfa->states = NULL;
	nfa->last_state = &nfa->states;

	if (symbol->type == SYMBOL_STATEMENT && symbol->content.symbol->type == SYMBOL_RULE) {
		nfa_rule_compile(nfa, symbol->content.symbol);
		return nfa;
	}

	statement = symbol->next;

	/* compile rules */
	while(statement != NULL) {
		if (statement->content.symbol->type == SYMBOL_RULE)
			nfa_rule_compile(nfa, statement->content.symbol);
		statement = statement->next;
	}

	statement = symbol->next;

	/* compile directives */
	while(statement != NULL) {
		switch(statement->content.symbol->type)
		{
			case SYMBOL_DIRECTIVE_START:
				nfa_directive_start_compile(nfa, statement->content.symbol);
				break;
			case SYMBOL_DIRECTIVE_END:
				nfa_directive_end_compile(nfa, statement->content.symbol);
				break;
		}
		statement = statement->next;
	}

	return nfa;
}
