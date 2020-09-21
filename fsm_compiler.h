#ifndef FSMCONV_FSM_COMPILER_H
#define FSMCONV_FSM_COMPILER_H 1

#include "fsm_types.h"
#include "parser_types.h"

struct fsm * fsm_compile(struct symbol * symbol);

#endif /* FSMCONV_FSM_COMPILER_H */
