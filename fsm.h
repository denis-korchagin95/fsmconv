#ifndef FSMCONV_NFA_H
#define FSMCONV_NFA_H 1

#include "fsm_types.h"

extern struct fsm_state * fsm_search_state_by_id(struct fsm * nfa, unsigned int id);
extern struct fsm_state * fsm_search_state_by_subset(struct fsm * nfa, struct fsm_state_list * states);

extern struct fsm_state_list * fsm_state_epsilon_closure(struct fsm_state * state);
extern struct fsm_state_list * fsm_epsilon_closure(struct fsm * nfa, struct fsm_state_list * states);

extern struct fsm * nfa_to_dfa(struct fsm * nfa);

extern unsigned int fsm_determine_type(struct fsm * fsm);

#endif /* FSMCONV_NFA_H */
