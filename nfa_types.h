#ifndef NFA2DFACONV_NFA_TYPES_H
#define NFA2DFACONV_NFA_TYPES_H 1

#include <stdint.h>

struct nfa_state_set {
  struct nfa_state_set * next;
  uint32_t element;
};

struct nfa_char_set {
  struct nfa_char_set * next;
  int ch;
};

struct nfa_transition {
  struct nfa_state_set * states;
  struct nfa_transition * next;
  int ch;
};

struct nfa_state {
  uint32_t id;
  uint32_t attrs;
  struct nfa_state_set * subset;
  struct nfa_transition * transitions;
  struct nfa_state * next;
};

struct nfa {
  struct nfa_state * states;
  struct nfa_state ** last_state;
  int nstates;
};

#endif /* NFA2DFACONV_NFA_TYPES_H */
