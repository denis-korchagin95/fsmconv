#ifndef NFA2DFACONV_PARSER_H
#define NFA2DFACONV_PARSER_H 1

extern struct token eof_token;

void init_parser(void);
struct symbol * parse(FILE * source); 

#endif /* NFA2DFACONV_PARSER_H */
