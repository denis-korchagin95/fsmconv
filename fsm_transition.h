#ifndef FSMCONV_FSM_TRANSITION_H
#define FSMCONV_FSM_TRANSITION_H 1

#include <stdbool.h>

#define EPSILON_CHAR (-1)

struct fsm_state_list;

struct fsm_transition {
  struct fsm_state_list * states;
  struct fsm_transition * next;
  int ch;
};

bool fsm_transition_has_state(struct fsm_transition * transition, unsigned int state_id);

#endif /* FSMCONV_FSM_TRANSITION_H */
