#ifndef FSMCONV_PRINT_H
#define FSMCONV_PRINT_H 1

#include <stdio.h>

struct token;
struct symbol;
struct fsm_state_list;
struct ast;

extern void print_token(FILE * output, struct token * token);
extern void print_symbol(FILE * output, struct symbol * symbol);

extern void print_fsm_state_list(FILE * output, struct fsm_state_list * list);
extern void print_ast(FILE * output, struct ast * ast);

#endif /* FSMCONV_PRINT_H */
