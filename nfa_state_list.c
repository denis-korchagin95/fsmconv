#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "nfa_types.h"
#include "parser_types.h"
#include "nfa_state_list.h"
#include "allocator.h"


struct nfa_state_list * nfa_state_list_create(uint32_t state_id)
{
    struct nfa_state_list * state_item = ___alloc_nfa_state_list();
    state_item->state_id = state_id;
    state_item->next = NULL;
    return state_item;
}

bool nfa_state_list_has_state(struct nfa_state_list * list, uint32_t state_id)
{
    while(list != NULL)
    {
        if (list->state_id == state_id)
            return true;
        list = list->next;
    }
    return false;
}
