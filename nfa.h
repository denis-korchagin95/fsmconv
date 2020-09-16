#ifndef NFA2DFA_CONV_NFA_H
#define NFA2DFA_CONV_NFA_H 1

bool nfa_transition_has_state(struct nfa_transition * transition, struct nfa_state * state);
struct nfa_transition * nfa_search_transition_by_character(struct nfa_state * state, int ch);
struct nfa_state * nfa_search_state_by_id(struct nfa * nfa, uint32_t id);
struct nfa_state * nfa_search_state_by_subset(struct nfa * nfa, struct nfa_state_list * states);

struct nfa_state_list * nfa_state_empty_closure(struct nfa_state * state);
struct nfa_state_list * nfa_empty_closure(struct nfa * nfa, struct nfa_state_list * states);

struct nfa * nfa_to_dfa(struct nfa * nfa);

#endif /* NFA2DFA_CONV_NFA_H */
