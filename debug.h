#ifndef FSMCONV_DEBUG_H
#define FSMCONV_DEBUG_H 1

#include <stdio.h>

struct token;
struct symbol;
struct fsm_state_list;
struct ast;

extern void debug_token(FILE * output, struct token * token);
extern void debug_symbol(FILE * output, struct symbol * symbol);

extern void debug_fsm_state_list(FILE * output, struct fsm_state_list * list);
extern void debug_ast(FILE * output, struct ast * ast);

#endif /* FSMCONV_DEBUG_H */
