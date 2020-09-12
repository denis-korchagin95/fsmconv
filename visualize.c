#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "nfa_types.h"
#include "util.h"
#include "visualize.h"

void visualize_nfa(struct nfa * nfa, FILE * output)
{
   fprintf(output, "digraph nfa {\n");

   struct nfa_state * s_iter = nfa->states;
   struct nfa_transition * t_iter;
   struct nfa_state_list * ss_iter;
   bool is_initial, is_finished;

   while(s_iter) {
	   is_initial = s_iter->attrs & NFA_STATE_ATTR_INITIAL;
	   is_finished = s_iter->attrs & NFA_STATE_ATTR_FINISHED;
	   if (is_initial && is_finished)
      		fprintf(output, "\ts%d [label=\"State '%s'\", shape=tripleoctagon];\n", s_iter->id, s_iter->name);
	   else if (is_initial && !is_finished)
      		fprintf(output, "\ts%d [label=\"State '%s'\", shape=invhouse];\n", s_iter->id, s_iter->name);
	   else if (!is_initial && is_finished)
      		fprintf(output, "\ts%d [label=\"State '%s'\", shape=doubleoctagon];\n", s_iter->id, s_iter->name);
	   else 
	   	fprintf(output, "\ts%d [label=\"State '%s'\"]; \n", s_iter->id, s_iter->name);

    t_iter = s_iter->transitions;

    while(t_iter) {
      ss_iter = t_iter->states;
      while(ss_iter) {
        fprintf(output, "\ts%d -> s%d [label=\"%s\"];\n", s_iter->id, ss_iter->state->id, char_to_string(t_iter->ch));
        ss_iter = ss_iter->next;
      }
      t_iter = t_iter->next;
    }

    s_iter = s_iter->next;
   }

   fprintf(output, "}\n");

   fflush(output);
}
