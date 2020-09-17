#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "nfa_types.h"
#include "parser_types.h"
#include "nfa.h"
#include "allocator.h"
#include "util.h"
#include "nfa_state.h"
#include "nfa_state_list.h"
#include "nfa_character_list.h"

struct nfa_state * nfa_search_state_by_id(struct nfa * nfa, uint32_t id)
{
    struct nfa_state * it = nfa->states;
    while(it != NULL) {
        if (it->id == id)
            return it;
        it = it->next;
    }
    return NULL;
}

struct nfa_state_list * nfa_empty_closure(struct nfa * nfa, struct nfa_state_list * states)
{
    struct nfa_state_list * empty_closure, * state_item, * hold, * stack, * current;
    struct nfa_state * state;
    struct nfa_transition * transition;

    empty_closure = NULL;
    stack = NULL;

    list_foreach(state_item, states) {
        hold = nfa_state_list_create(state_item->state_id);
        nfa_state_list_ordered_insert(&empty_closure, hold);

        hold = nfa_state_list_create(state_item->state_id);
        hold->next = stack;
        stack = hold;
    }

    while (stack != NULL) {
        hold = stack->next;
        current = stack;
        stack = hold;

        state = nfa_search_state_by_id(nfa, current->state_id);
        if (state == NULL) {
            continue;
        }

        transition = nfa_state_search_transition_by_character(state, EMPTY_CHAR);

        if (transition == NULL)
            continue;

        list_foreach(state_item, transition->states) {
            if (! nfa_state_list_has_state(empty_closure, state_item->state_id)) {
                hold = nfa_state_list_create(state_item->state_id);
                nfa_state_list_ordered_insert(&empty_closure, hold);

                hold = nfa_state_list_create(state_item->state_id);
                hold->next = stack;
                stack = hold;
            }
        }

        /* TODO: free(current) */
    }

    return empty_closure;
}

struct nfa_state_list * nfa_state_empty_closure(struct nfa_state * state)
{
    struct nfa_state_list * empty_closure, * stack, * hold, * current, * state_item;
    struct nfa_state * search_state;
    struct nfa_transition * transition;

    empty_closure = nfa_state_list_create(state->id);
    stack = nfa_state_list_create(state->id);

    while(stack != NULL)
    {
        hold = stack->next;
        current = stack;
        stack = hold;

        search_state = current->state_id == state->id ? state : nfa_search_state_by_id(state->owner, current->state_id);

        if (search_state == NULL)
            continue;

        transition = nfa_state_search_transition_by_character(search_state, EMPTY_CHAR);

        if (transition == NULL)
            continue;

        list_foreach(state_item, transition->states) {
            if (! nfa_state_list_has_state(empty_closure, state_item->state_id)) {
                hold = nfa_state_list_create(state_item->state_id);
                nfa_state_list_ordered_insert(&empty_closure, hold);

                hold = nfa_state_list_create(state_item->state_id);
                hold->next = stack;
                stack = hold;
            }
        }

        /* TODO: free(current) */
    }

    return empty_closure;
}

static struct nfa_state * nfa_find_not_visited_state(struct nfa * nfa)
{
    struct nfa_state * state;
    list_foreach(state, nfa->states) {
        if (! (state->attrs & NFA_STATE_ATTR_VISITED))
            return state;
    }
    return NULL;
}

struct nfa * nfa_to_dfa(struct nfa * nfa)
{
    struct nfa * dfa;
    struct nfa_state * state, * new_state, * search_state;
    struct nfa_state_list * empty_closure, * state_list, * state_item, * state_item2, * new_state_item, * finished_states;
    struct nfa_character_list * characters, * character_item;
    struct nfa_transition * transition;

    dfa = ___alloc_nfa();
    dfa->states = NULL;
    dfa->last_state = &dfa->states;
    dfa->state_count = 0;

    characters = NULL;
    finished_states = NULL;

    list_foreach(state, nfa->states) {
        if (state->attrs & NFA_STATE_ATTR_INITIAL) {
            new_state = ___alloc_nfa_state();
            new_state->subset = nfa_state_empty_closure(state);
            new_state->owner = dfa;
            new_state->id = dfa->state_count++;
            new_state->next = NULL;
            new_state->transitions = NULL;
            new_state->attrs = NFA_STATE_ATTR_INITIAL;

            *dfa->last_state = new_state;
            dfa->last_state = &new_state->next;
        }

	    if(state->attrs & NFA_STATE_ATTR_FINISHED) {
		    state_item = ___alloc_nfa_state_list();
		    state_item->state_id = state->id;
		    state_item->next = finished_states;
		    finished_states = state_item;
	    }

        list_foreach(transition, state->transitions) {
            if (transition->ch != EMPTY_CHAR && !nfa_character_list_has_character(characters, transition->ch)) {
                character_item = ___alloc_nfa_character_list();
                character_item->ch = transition->ch;

                character_item->next = characters;
                characters = character_item;
            }
        }
    }

    while((state = nfa_find_not_visited_state(dfa)) != NULL) {
        state->attrs |= NFA_STATE_ATTR_VISITED;

        if (! (state->attrs & NFA_STATE_ATTR_FINISHED)) {
            list_foreach(state_item, state->subset) {
                if (nfa_state_list_has_state(finished_states, state_item->state_id)) {
                    state->attrs |= NFA_STATE_ATTR_FINISHED;
                    break;
                }
            }
        }

        list_foreach(character_item, characters) {
            state_list = NULL;
            list_foreach(state_item, state->subset) {
                search_state = nfa_search_state_by_id(nfa, state_item->state_id);
                if (search_state == NULL)
                    continue;
                transition = nfa_state_search_transition_by_character(search_state, character_item->ch);
                if (transition == NULL)
                    continue;
                list_foreach(state_item2, transition->states) {
                    new_state_item = nfa_state_list_create(state_item2->state_id);
                    new_state_item->next = state_list;
                    state_list = new_state_item;
                }
            }

            if (state_list == NULL)
                continue;

            empty_closure = nfa_empty_closure(nfa, state_list);

            if (nfa_search_state_by_subset(dfa, empty_closure) == NULL) {
                new_state = ___alloc_nfa_state();
                new_state->subset = empty_closure;
                new_state->owner = dfa;
                new_state->id = dfa->state_count++;
                new_state->next = NULL;
                new_state->transitions = NULL;
                new_state->attrs = 0;

                *dfa->last_state = new_state;
                dfa->last_state = &new_state->next;
            }

            /* TODO: free(state_list) */
            state_list = NULL;

            list_foreach(state_item, empty_closure) {
                new_state_item = nfa_state_list_create(state_item->state_id);
                new_state_item->next = state_list;
                state_list = new_state_item;
            }

            transition = nfa_state_search_transition_by_character(state, character_item->ch);

            if (transition == NULL) {
                transition = ___alloc_nfa_transition();
                transition->states = state_list;
                transition->ch = character_item->ch;

                transition->next = state->transitions;
                state->transitions = transition;
            }
            else {
                /* TODO: free(transition->states); */
                transition->states = state_list;
            }
        }
    }

    return dfa;
}

struct nfa_state * nfa_search_state_by_subset(struct nfa * nfa, struct nfa_state_list * states)
{
    struct nfa_state * it;
    list_foreach(it, nfa->states) {
        if (nfa_state_list_equals(it->subset, states))
            return it;
    }
    return NULL;
}
