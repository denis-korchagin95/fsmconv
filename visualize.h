#ifndef FSMCONV_VISUALIZE_H
#define FSMCONV_VISUALIZE_H 1

void visualize_nfa(FILE * output, struct fsm * nfa);

void generate_nfa_language(FILE * output, struct fsm * nfa);
void generate_dfa_language(FILE * output, struct fsm * dfa);

#endif /* FSMCONV_VISUALIZE_H */
