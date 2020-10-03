#ifndef FSMCONV_NFA_H
#define FSMCONV_NFA_H 1

#define FSM_TYPE_UNDEFINED          (0)
#define FSM_TYPE_EPSILON_NFA        (1)
#define FSM_TYPE_NFA                (2)
#define FSM_TYPE_FAKE_DFA           (3)
#define FSM_TYPE_DFA                (4)

struct fsm_state_list;
struct fsm_state;

struct fsm {
    struct fsm_state * states;
    struct fsm_state ** last_state;
    unsigned int state_count;
    unsigned int type;
};

extern struct fsm_state * fsm_search_state_by_id(struct fsm * fsm, unsigned int id);
extern struct fsm_state * fsm_search_state_by_subset(struct fsm * fsm, struct fsm_state_list * states);

extern struct fsm_state_list * fsm_state_epsilon_closure(struct fsm_state * state);
extern struct fsm_state_list * fsm_epsilon_closure(struct fsm * fsm, struct fsm_state_list * states);

extern struct fsm * nfa_to_dfa(struct fsm * fsm);

extern unsigned int fsm_determine_type(struct fsm * fsm);

#endif /* FSMCONV_NFA_H */
