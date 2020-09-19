#ifndef FSMCONV_TYPES_H
#define FSMCONV_TYPES_H 1

#define EMPTY_CHAR (-1)

#define FSM_STATE_ATTR_INITIAL      bit(0)
#define FSM_STATE_ATTR_FINISHED     bit(1)
#define FSM_STATE_ATTR_VISITED      bit(2)

#define FSM_TYPE_UNDEFINED          0
#define FSM_TYPE_EPSILON_NFA        1
#define FSM_TYPE_NFA                2
#define FSM_TYPE_FAKE_DFA           3
#define FSM_TYPE_DFA                4

struct fsm_state_list
{
    struct fsm_state_list * next;
    uint32_t state_id;
};

struct fsm_transition {
  struct fsm_state_list * states;
  struct fsm_transition * next;
  int ch;
};

struct fsm_state {
    struct fsm_state * next;
    struct fsm * fsm;
    struct fsm_transition * transitions;
    struct fsm_state_list * subset;
    const char * name;
    uint32_t id;
    uint32_t attrs;
};

struct fsm {
    struct fsm_state * states;
    struct fsm_state ** last_state;
    uint32_t state_count;
    uint32_t type;
};

#endif /* FSMCONV_TYPES_H */
