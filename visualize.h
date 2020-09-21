#ifndef FSMCONV_VISUALIZE_H
#define FSMCONV_VISUALIZE_H 1

#include <stdio.h>

#include "fsm_types.h"

void visualize_nfa(FILE * output, struct fsm * nfa);
void visualize_dfa(FILE * output, struct fsm * dfa);

void generate_nfa_language(FILE * output, struct fsm * nfa);
void generate_dfa_language(FILE * output, struct fsm * dfa);

#endif /* FSMCONV_VISUALIZE_H */
