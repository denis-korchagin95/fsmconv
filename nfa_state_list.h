#ifndef NFA2DFA_CONV_NFA_STATE_LIST_H
#define NFA2DFA_CONV_NFA_STATE_LIST_H 1

struct nfa_state_list * nfa_state_list_create(uint32_t state_id);
void nfa_state_list_ordered_insert(struct nfa_state_list ** list, struct nfa_state_list * new_item);

bool nfa_state_list_contains(struct nfa_state_list * first, struct nfa_state_list * second);
uint32_t nfa_state_list_length(struct nfa_state_list * list);
bool nfa_state_list_equals(struct nfa_state_list * first, struct nfa_state_list * second);

bool nfa_state_list_has_state(struct nfa_state_list * list, uint32_t state_id);

#endif /* NFA2DFA_CONV_NFA_STATE_LIST_H */
