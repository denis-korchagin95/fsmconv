#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "nfa_types.h"
#include "nfa_transition.h"


bool nfa_transition_has_state(struct nfa_transition * transition, uint32_t state_id)
{
    struct nfa_state_list * it = transition->states;
    while(it != NULL)
    {
        if (it->state_id == state_id)
            return true;
        it = it->next;
    }
    return false;
}
