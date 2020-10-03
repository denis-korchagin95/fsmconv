#ifndef FSMCONV_FSM_COMPILER_H
#define FSMCONV_FSM_COMPILER_H 1

struct ast;
struct fsm;

struct fsm * fsm_compile(struct ast * tree);

#endif /* FSMCONV_FSM_COMPILER_H */
