#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "fsm_types.h"
#include "fsm_transition.h"


bool fsm_transition_has_state(struct fsm_transition * transition, uint32_t state_id)
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
