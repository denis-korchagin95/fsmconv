#ifndef FSMCONV_DEBUG_H
#define FSMCONV_DEBUG_H 1

#include <stdio.h>

#include "fsm_types.h"
#include "parser_types.h"

void debug_token(FILE * output, struct token * token);
void debug_symbol(FILE * output, struct symbol * symbol, int depth);

void debug_fsm_state_list(FILE * output, struct fsm_state_list * list);

#endif /* FSMCONV_DEBUG_H */
