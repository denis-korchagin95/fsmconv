#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "nfa_types.h"
#include "parser_types.h"
#include "allocator.h"

static struct nfa_state * nfa_search_state_by_name(struct nfa * nfa, const char * name)
{
	struct nfa_state * it = nfa->states;
	while(it != NULL) {
		if(strcmp(name, it->name) == 0)
			return it;
		it = it->next;
	}
	return NULL;
}

static struct nfa_transition * nfa_search_transition_by_character(struct nfa_state * state, int ch)
{
	struct nfa_transition * it = state->transitions;
	while(it != NULL)
	{
		if (it->ch == ch)
			return it;
		it = it->next;
	}
	return NULL;
}

static bool nfa_transition_has_state(struct nfa_transition * transition, struct nfa_state * state)
{
	struct nfa_state_list * it = transition->states;
	while(it != NULL)
	{
		if (it->state == state)
			return true;
		it = it->next;
	}
	return false;
}

static struct nfa_state * nfa_state_compile(struct nfa * nfa, struct symbol * symbol)
{
	struct nfa_state * state;

	state = nfa_search_state_by_name(nfa, symbol->content.identifier->name);

	if (state != NULL)
		return state;

	state = ___alloc_nfa_state();
	state->id = symbol->content.state.id;
	state->name = symbol->content.state.identifier->name;
	state->subset = NULL;
	state->transitions = NULL;
	state->next = NULL;

	(*nfa->last_state) = state;
	nfa->last_state = &state->next;

	return state;
}

static void nfa_transition_compile(struct nfa_state * source, struct nfa_state * target, struct symbol * character)
{
	struct nfa_transition * transition;
	struct nfa_state_list * state_list;

	transition = nfa_search_transition_by_character(source, character->content.code);

	if (transition == NULL)
	{
		transition = ___alloc_nfa_transition();
		transition->states = NULL;
		transition->next = source->transitions;
		transition->ch = character->type == SYMBOL_SPECIAL_CHARACTER_BUILTIN ? character->content.special_character.value->content.code : character->content.code;

		source->transitions = transition;
	}

	if (! nfa_transition_has_state(transition, target)) {
		state_list = ___alloc_nfa_state_list();
		state_list->state = target;
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


struct nfa * nfa_compile(struct symbol * symbol)
{
	struct nfa * nfa;
	struct symbol * statement;

	nfa = ___alloc_nfa();
	nfa->states = NULL;
	nfa->last_state = &nfa->states;

	if (symbol->type == SYMBOL_STATEMENT && symbol->content.symbol->type == SYMBOL_RULE) {
		nfa_rule_compile(nfa, symbol->content.symbol);
		return;
	}

	statement = symbol->next;

	while(statement != NULL) {
		if (statement->content.symbol->type == SYMBOL_RULE)
			nfa_rule_compile(nfa, statement->content.symbol);
		statement = statement->next;
	}

	return nfa;
}
