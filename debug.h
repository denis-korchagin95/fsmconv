#ifndef NFA2DFACONV_DEBUG_H
#define NFA2DFACONV_DEBUG_H 1

void debug_token(FILE * output, struct token * token);
void debug_symbol(FILE * output, struct symbol * symbol, int depth);

#endif /* NFA2DFACONV_DEBUG_H */
