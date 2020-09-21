#ifndef FSMCONV_FSM_STATE_LIST_H
#define FSMCONV_FSM_STATE_LIST_H 1

#include <stdint.h>
#include <stdbool.h>

#include "fsm_types.h"

struct fsm_state_list * fsm_state_list_create(uint32_t state_id);
void fsm_state_list_ordered_insert(struct fsm_state_list ** list, struct fsm_state_list * new_item);

bool fsm_state_list_contains(struct fsm_state_list * first, struct fsm_state_list * second);
uint32_t fsm_state_list_length(struct fsm_state_list * list);
bool fsm_state_list_equals(struct fsm_state_list * first, struct fsm_state_list * second);

bool fsm_state_list_has_state(struct fsm_state_list * list, uint32_t state_id);

#endif /* FSMCONV_FSM_STATE_LIST_H */
