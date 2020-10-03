#include "fsm_transition.h"
#include "fsm_state_list.h"

#include <stddef.h>

bool fsm_transition_has_state(struct fsm_transition * transition, unsigned int state_id)
{
    struct fsm_state_list * it = transition->states;
    while(it != NULL)
    {
        if (it->state_id == state_id)
            return true;
        it = it->next;
    }
    return false;
}
