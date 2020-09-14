#ifndef NFA2DFA_CONV_PARSER_H
#define NFA2DFA_CONV_PARSER_H 1

extern struct token eof_token;

void init_parser(void);
struct symbol * parse(FILE * source); 

#endif /* NFA2DFA_CONV_PARSER_H */
