#include <stdint.h>
#include <stddef.h>

#include "nfa_types.h"
#include "nfa_state.h"


struct nfa_transition * nfa_state_search_transition_by_character(struct nfa_state * state, int ch)
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
