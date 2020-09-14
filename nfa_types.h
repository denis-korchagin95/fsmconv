#ifndef NFA2DFACONV_NFA_TYPES_H
#define NFA2DFACONV_NFA_TYPES_H 1

#define EMPTY_CHAR (-1)

#define NFA_STATE_ATTR_INITIAL      BIT(0)
#define NFA_STATE_ATTR_FINISHED     BIT(1)
#define NFA_STATE_ATTR_VISITED      BIT(2)

struct nfa_state_set {
  struct nfa_state_set * next;
  uint32_t element;
};

struct nfa_state_list
{
    struct nfa_state_list * next;
    uint32_t state_id;
};

struct nfa_character_list
{
    struct nfa_character_list * next;
	int ch;
};

struct nfa_char_set {
  struct nfa_char_set * next;
  int ch;
};

struct nfa_transition {
  struct nfa_state_list * states;
  struct nfa_transition * next;
  int ch;
};

struct nfa_state {
  uint32_t id;
  const char * name;
  uint32_t attrs;
  struct nfa_state_set * subset;
  struct nfa_transition * transitions;
  struct nfa_state * next;
};

struct nfa {
  struct nfa_state * states;
  struct nfa_state ** last_state;
};

#endif /* NFA2DFACONV_NFA_TYPES_H */
