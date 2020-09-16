#ifndef NFA2DFA_CONV_VISUALIZE_H
#define NFA2DFA_CONV_VISUALIZE_H 1

void visualize_nfa(FILE * output, struct nfa * nfa);

void generate_nfa_language(FILE * output, struct nfa * nfa);
void generate_dfa_language(FILE * output, struct nfa * dfa);

#endif /* NFA2DFA_CONV_VISUALIZE_H */
