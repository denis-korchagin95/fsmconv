#ifndef NFA2DFA_CONV_NFA_STATE_LIST_H
#define NFA2DFA_CONV_NFA_STATE_LIST_H 1

struct nfa_state_list * nfa_state_list_create(uint32_t state_id);
void nfa_state_list_ordered_insert(struct nfa_state_list ** list, struct nfa_state_list * new_item);

bool nfa_state_list_has_state(struct nfa_state_list * list, uint32_t state_id);

#endif /* NFA2DFA_CONV_NFA_STATE_LIST_H */
