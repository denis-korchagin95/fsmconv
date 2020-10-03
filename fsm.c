#include "fsm.h"
#include "fsm_state_list.h"
#include "fsm_state.h"
#include "internal_allocators.h"
#include "character_list.h"

#include <stddef.h>

struct fsm_state * fsm_search_state_by_id(struct fsm * fsm, unsigned int id)
{
    struct fsm_state * it = fsm->states;
    while(it != NULL) {
        if (it->id == id)
            return it;
        it = it->next;
    }
    return NULL;
}

struct fsm_state_list * fsm_epsilon_closure(struct fsm * fsm, struct fsm_state_list * states)
{
    struct fsm_state_list * epsilon_closure, * state_item, * hold, * stack, * current;
    struct fsm_state * state;
    struct fsm_transition * transition;

    epsilon_closure = NULL;
    stack = NULL;

    list_foreach(state_item, states) {
        hold = fsm_state_list_create(state_item->state_id);
        fsm_state_list_ordered_insert(&epsilon_closure, hold);

        hold = fsm_state_list_create(state_item->state_id);
        hold->next = stack;
        stack = hold;
    }

    while (stack != NULL) {
        hold = stack->next;
        current = stack;
        stack = hold;

        state = fsm_search_state_by_id(fsm, current->state_id);
        if (state == NULL) {
            continue;
        }

        transition = fsm_state_search_transition_by_character(state, EPSILON_CHAR);

        if (transition == NULL)
            continue;

        list_foreach(state_item, transition->states) {
            if (! fsm_state_list_has_state(epsilon_closure, state_item->state_id)) {
                hold = fsm_state_list_create(state_item->state_id);
                fsm_state_list_ordered_insert(&epsilon_closure, hold);

                hold = fsm_state_list_create(state_item->state_id);
                hold->next = stack;
                stack = hold;
            }
        }

        /* TODO: free(current) */
    }

    return epsilon_closure;
}

struct fsm_state_list * fsm_state_epsilon_closure(struct fsm_state * state)
{
    struct fsm_state_list * epsilon_closure, * stack, * hold, * current, * state_item;
    struct fsm_state * search_state;
    struct fsm_transition * transition;

    epsilon_closure = fsm_state_list_create(state->id);
    stack = fsm_state_list_create(state->id);

    while(stack != NULL)
    {
        hold = stack->next;
        current = stack;
        stack = hold;

        search_state = current->state_id == state->id ? state : fsm_search_state_by_id(state->fsm, current->state_id);

        if (search_state == NULL)
            continue;

        transition = fsm_state_search_transition_by_character(search_state, EPSILON_CHAR);

        if (transition == NULL)
            continue;

        list_foreach(state_item, transition->states) {
            if (! fsm_state_list_has_state(epsilon_closure, state_item->state_id)) {
                hold = fsm_state_list_create(state_item->state_id);
                fsm_state_list_ordered_insert(&epsilon_closure, hold);

                hold = fsm_state_list_create(state_item->state_id);
                hold->next = stack;
                stack = hold;
            }
        }

        /* TODO: free(current) */
    }

    return epsilon_closure;
}

static struct fsm_state * fsm_find_not_visited_state(struct fsm * fsm)
{
    struct fsm_state * state;
    list_foreach(state, fsm->states) {
        if (! (state->attrs & FSM_STATE_ATTR_VISITED))
            return state;
    }
    return NULL;
}

struct fsm * nfa_to_dfa(struct fsm * nfa)
{
    struct fsm * dfa;
    struct fsm_state * state, * new_state, * search_state;
    struct fsm_state_list * epsilon_closure, * state_list, * state_item, * state_item2, * new_state_item, * finished_states;
    struct character_list * characters, * character_item;
    struct fsm_transition * transition;

    dfa = alloc_fsm();
    dfa->type = FSM_TYPE_DFA;
    dfa->states = NULL;
    dfa->last_state = &dfa->states;
    dfa->state_count = 0;

    characters = NULL;
    finished_states = NULL;

    list_foreach(state, nfa->states) {
        if (state->attrs & FSM_STATE_ATTR_INITIAL) {
            new_state = alloc_fsm_state();
            new_state->subset = fsm_state_epsilon_closure(state);
            new_state->fsm = dfa;
            new_state->id = dfa->state_count++;
            new_state->next = NULL;
            new_state->transitions = NULL;
            new_state->attrs = FSM_STATE_ATTR_INITIAL;

            *dfa->last_state = new_state;
            dfa->last_state = &new_state->next;
        }

	    if(state->attrs & FSM_STATE_ATTR_FINAL) {
		    state_item = alloc_fsm_state_list();
		    state_item->state_id = state->id;
		    state_item->next = finished_states;
		    finished_states = state_item;
	    }

        list_foreach(transition, state->transitions) {
            if (transition->ch != EPSILON_CHAR && !character_list_has_character(characters, transition->ch)) {
                character_item = alloc_character_list();
                character_item->ch = transition->ch;

                character_item->next = characters;
                characters = character_item;
            }
        }
    }

    while((state = fsm_find_not_visited_state(dfa)) != NULL) {
        state->attrs |= FSM_STATE_ATTR_VISITED;

        if (! (state->attrs & FSM_STATE_ATTR_FINAL)) {
            list_foreach(state_item, state->subset) {
                if (fsm_state_list_has_state(finished_states, state_item->state_id)) {
                    state->attrs |= FSM_STATE_ATTR_FINAL;
                    break;
                }
            }
        }

        list_foreach(character_item, characters) {
            state_list = NULL;
            list_foreach(state_item, state->subset) {
                search_state = fsm_search_state_by_id(nfa, state_item->state_id);
                if (search_state == NULL)
                    continue;
                transition = fsm_state_search_transition_by_character(search_state, character_item->ch);
                if (transition == NULL)
                    continue;
                list_foreach(state_item2, transition->states) {
                    new_state_item = fsm_state_list_create(state_item2->state_id);
                    new_state_item->next = state_list;
                    state_list = new_state_item;
                }
            }

            if (state_list == NULL)
                continue;

            epsilon_closure = fsm_epsilon_closure(nfa, state_list);
            /* TODO: free(state_list) */
            state_list = NULL;

            search_state = fsm_search_state_by_subset(dfa, epsilon_closure);

            if (search_state == NULL) {
                new_state = alloc_fsm_state();
                new_state->subset = epsilon_closure;
                new_state->fsm = dfa;
                new_state->id = dfa->state_count++;
                new_state->next = NULL;
                new_state->transitions = NULL;
                new_state->attrs = 0;

                search_state = new_state;

                *dfa->last_state = new_state;
                dfa->last_state = &new_state->next;
            }

            transition = fsm_state_search_transition_by_character(state, character_item->ch);

            if (transition == NULL) {
                transition = alloc_fsm_transition();
                transition->states = fsm_state_list_create(search_state->id);
                transition->ch = character_item->ch;

                transition->next = state->transitions;
                state->transitions = transition;
            }
            else {
                /* TODO: free(transition->states); */
                transition->states = fsm_state_list_create(search_state->id);
            }
        }
    }

    /* TODO: free(finished_states); */
    /* TODO: free(characters); */

    return dfa;
}

struct fsm_state * fsm_search_state_by_subset(struct fsm * fsm, struct fsm_state_list * states)
{
    struct fsm_state * it;
    list_foreach(it, fsm->states) {
        if (fsm_state_list_equals(it->subset, states))
            return it;
    }
    return NULL;
}

unsigned int fsm_determine_type(struct fsm * fsm)
{
	struct fsm_state * state;
	struct fsm_transition * transition;

	bool has_epsilon_transition = false;
	bool has_multiple_transitions_by_character = false;

    list_foreach(state, fsm->states) {
        list_foreach(transition, state->transitions) {
            if (!has_epsilon_transition && transition->ch == EPSILON_CHAR)
				has_epsilon_transition = true;
			if (!has_multiple_transitions_by_character && transition->states != NULL && transition->states->next != NULL)
				has_multiple_transitions_by_character = true;
        }
		if(has_epsilon_transition || has_multiple_transitions_by_character)
			break;
    }

	if (has_epsilon_transition)
		return FSM_TYPE_EPSILON_NFA;
	else if (has_multiple_transitions_by_character)
		return FSM_TYPE_NFA;

	struct character_list * characters = NULL, * character_item;

	list_foreach(state, fsm->states) {
		list_foreach(transition, state->transitions) {
			if(!character_list_has_character(characters, transition->ch))
			{
				character_item = alloc_character_list();
				character_item->ch = transition->ch;
				character_item->next = characters;

				characters = character_item;
			}
		}
	}

	bool has_missed_transitions = false;

	list_foreach(character_item, characters) {
		list_foreach(state, fsm->states) {
			if (fsm_state_search_transition_by_character(state, character_item->ch) == NULL) {
				has_missed_transitions = true;
				goto out_loop;
			}
		}
	}

out_loop:
	/* TODO: free(characters); */

	if(has_missed_transitions)
		return FSM_TYPE_FAKE_DFA;

	return FSM_TYPE_DFA;
}
