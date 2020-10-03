#include "fsm_state.h"
#include "fsm_transition.h"

#include <stddef.h>

struct fsm_transition * fsm_state_search_transition_by_character(struct fsm_state * state, int ch)
{
    struct fsm_transition * it = state->transitions;
    while(it)
    {
        if (it->ch == ch)
            return it;
        it = it->next;
    }
    return NULL;
}
