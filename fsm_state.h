#ifndef FSMCONV_FSM_STATE_H
#define FSMCONV_FSM_STATE_H 1

#include "util.h"

#define FSM_STATE_ATTR_INITIAL  bit(0)
#define FSM_STATE_ATTR_FINAL    bit(1)
#define FSM_STATE_ATTR_VISITED  bit(2)

struct fsm_transition;
struct fsm_state_list;
struct fsm;

struct fsm_state {
	struct fsm_state * next;
	struct fsm * fsm;
	struct fsm_transition * transitions;
	struct fsm_state_list * subset;
	const char * name;
	unsigned int id;
	unsigned int attrs;
};

extern struct fsm_transition * fsm_state_search_transition_by_character(struct fsm_state * state, int ch);

#endif /* FSMCONV_FSM_STATE_H */
