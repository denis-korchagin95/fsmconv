#ifndef NFA2DFA_CONV_NFA_TYPES_H
#define NFA2DFA_CONV_NFA_TYPES_H 1

#define EMPTY_CHAR (-1)

#define NFA_STATE_ATTR_INITIAL      bit(0)
#define NFA_STATE_ATTR_FINISHED     bit(1)
#define NFA_STATE_ATTR_VISITED      bit(2)

struct nfa_state_list
{
    struct nfa_state_list * next;
    uint32_t state_id;
};

struct nfa_transition {
  struct nfa_state_list * states;
  struct nfa_transition * next;
  int ch;
};

struct nfa_state {
    struct nfa_state * next;
    struct nfa * owner;
    struct nfa_transition * transitions;
    struct nfa_state_list * subset;
    const char * name;
    uint32_t id;
    uint32_t attrs;
};

struct nfa {
    struct nfa_state * states;
    struct nfa_state ** last_state;
    uint32_t state_count;
};

#endif /* NFA2DFA_CONV_NFA_TYPES_H */
