#ifndef NFA2DFA_CONV_NFA_STATE_H
#define NFA2DFA_CONV_NFA_STATE_H 1

struct nfa_transition * nfa_state_search_transition_by_character(struct nfa_state * state, int ch);

#endif /* NFA2DFA_CONV_NFA_STATE_H */
