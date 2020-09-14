#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "nfa_types.h"
#include "parser_types.h"
#include "nfa_state_list.h"
#include "allocator.h"


void nfa_state_list_ordered_insert(struct nfa_state_list ** list, struct nfa_state_list * new_item)
{
    struct nfa_state_list ** it = list;
    while(*it != NULL) {
        if (new_item->state_id <= (*it)->state_id)
            break;
        it = &(*it)->next;
    }
    if (*it == NULL)
        *it = new_item;
    else {
        new_item->next = *it;
        *it = new_item;
    }
}

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
