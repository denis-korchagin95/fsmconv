#ifndef FSMCONV_PARSER_H
#define FSMCONV_PARSER_H 1

extern struct token eof_token;

void init_parser(void);
uint32_t parser_last_state_id(void);
struct symbol * parse(FILE * source); 

#endif /* FSMCONV_PARSER_H */
