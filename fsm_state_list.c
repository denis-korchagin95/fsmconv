#include "fsm_state_list.h"
#include "internal_allocators.h"

#include <stddef.h>

void fsm_state_list_ordered_insert(struct fsm_state_list ** list, struct fsm_state_list * new_item)
{
	struct fsm_state_list ** it = list;
	while(*it) {
		if (new_item->state_id <= (*it)->state_id)
			break;
		it = &(*it)->next;
	}
	if (! *it)
		*it = new_item;
	else {
		new_item->next = *it;
		*it = new_item;
	}
}

struct fsm_state_list * fsm_state_list_create(unsigned int state_id)
{
	struct fsm_state_list * state_item = alloc_fsm_state_list();
	state_item->state_id = state_id;
	state_item->next = NULL;
	return state_item;
}

bool fsm_state_list_has_state(struct fsm_state_list * list, unsigned int state_id)
{
	while(list)
	{
		if (list->state_id == state_id)
			return true;
		list = list->next;
	}
	return false;
}

bool fsm_state_list_contains(struct fsm_state_list * first, struct fsm_state_list * second)
{
	while (second) {
		if (! fsm_state_list_has_state(first, second->state_id)) {
			return false;
		}
		second = second->next;
	}
	return true;
}

unsigned int fsm_state_list_length(struct fsm_state_list * list)
{
	unsigned int len = 0;
	while (list) {
		list = list->next;
		++len;
	}
	return len;
}

bool fsm_state_list_equals(struct fsm_state_list * first, struct fsm_state_list * second)
{
	return fsm_state_list_length(first) == fsm_state_list_length(second) && fsm_state_list_contains(first, second);
}
