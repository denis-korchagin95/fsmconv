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
            hold = nfa_state_list_create(state_item->state_id);
            hold->next = stack;
            stack = hold;

            if (! nfa_state_list_has_state(empty_closure, state_item->state_id)) {
                hold = nfa_state_list_create(state_item->state_id);
                hold->next = empty_closure;
                empty_closure = hold;
            }
        }

        /* TODO: free(current) */
    }

    return empty_closure;
}
