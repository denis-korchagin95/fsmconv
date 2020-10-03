#ifndef FSMCONV_VISUALIZE_H
#define FSMCONV_VISUALIZE_H 1

#include <stdio.h>

struct fsm;

extern void visualize_nfa(FILE * output, struct fsm * nfa);
extern void visualize_dfa(FILE * output, struct fsm * dfa);

extern void generate_nfa_language(FILE * output, struct fsm * nfa);
extern void generate_dfa_language(FILE * output, struct fsm * dfa);

#endif /* FSMCONV_VISUALIZE_H */
