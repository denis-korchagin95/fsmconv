#ifndef FSMCONV_PARSER_H
#define FSMCONV_PARSER_H 1

#include <stdio.h>

struct token;
struct ast;

extern void init_symbols();
extern struct token * parse(struct token * token, struct ast ** tree); 
extern unsigned int parser_last_state_id(void);

#endif /* FSMCONV_PARSER_H */
