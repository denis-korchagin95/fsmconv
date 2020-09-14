#ifndef NFA2DFACONV_DEBUG_H
#define NFA2DFACONV_DEBUG_H 1

void debug_token(FILE * output, struct token * token);
void debug_symbol(FILE * output, struct symbol * symbol, int depth);

void debug_nfa_state_list(FILE * output, struct nfa_state_list * list);

#endif /* NFA2DFACONV_DEBUG_H */
