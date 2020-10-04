#ifndef FSMCONV_FSM_STATE_LIST_H
#define FSMCONV_FSM_STATE_LIST_H 1

#include <stdbool.h>

struct fsm_state_list
{
	struct fsm_state_list * next;
	unsigned int state_id;
};

extern struct fsm_state_list * fsm_state_list_create(unsigned int state_id);
extern void fsm_state_list_ordered_insert(struct fsm_state_list ** list, struct fsm_state_list * new_item);

extern bool fsm_state_list_contains(struct fsm_state_list * first, struct fsm_state_list * second);
extern unsigned int fsm_state_list_length(struct fsm_state_list * list);
extern bool fsm_state_list_equals(struct fsm_state_list * first, struct fsm_state_list * second);

extern bool fsm_state_list_has_state(struct fsm_state_list * list, unsigned int state_id);

#endif /* FSMCONV_FSM_STATE_LIST_H */
