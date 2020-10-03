#ifndef FSMCONV_FSM_TRANSITION_H
#define FSMCONV_FSM_TRANSITION_H 1

#include <stdbool.h>

#include "fsm_types.h"

bool fsm_transition_has_state(struct fsm_transition * transition, unsigned int state_id);

#endif /* FSMCONV_FSM_TRANSITION_H */
