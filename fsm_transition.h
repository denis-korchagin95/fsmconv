#ifndef FSMCONV_FSM_TRANSITION_H
#define FSMCONV_FSM_TRANSITION_H 1

#include <stdint.h>
#include <stdbool.h>

#include "fsm_types.h"

bool fsm_transition_has_state(struct fsm_transition * transition, uint32_t state_id);

#endif /* FSMCONV_FSM_TRANSITION_H */
