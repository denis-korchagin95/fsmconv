#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "nfa_types.h"
#include "util.h"
#include "visualize.h"

void visualize_nfa(struct nfa * nfa, FILE * output)
{
	struct nfa_state * state;
	struct nfa_transition * transition;
	struct nfa_state_list * state_list;
	bool is_initial, is_finished;

	fprintf(output, "digraph nfa {\n");

	state = nfa->states;

	while(state != NULL) {
		is_initial = state->attrs & NFA_STATE_ATTR_INITIAL;
		is_finished = state->attrs & NFA_STATE_ATTR_FINISHED;

		if (is_initial && is_finished)
			fprintf(output, "\ts%d [label=\"State '%s'\", shape=tripleoctagon];\n", state->id, state->name);
		else if (is_initial && !is_finished)
			fprintf(output, "\ts%d [label=\"State '%s'\", shape=invhouse];\n", state->id, state->name);
		else if (!is_initial && is_finished)
			fprintf(output, "\ts%d [label=\"State '%s'\", shape=doubleoctagon];\n", state->id, state->name);
		else
			fprintf(output, "\ts%d [label=\"State '%s'\"]; \n", state->id, state->name);

		transition = state->transitions;

		while(transition != NULL) {
			state_list = transition->states;
			while(state_list != NULL) {
				fprintf(output, "\ts%d -> s%d [label=\"%s\"];\n", state->id, state_list->state->id, char_to_string(transition->ch));
				state_list = state_list->next;
			}
			transition = transition->next;
		}

		state = state->next;
	}

	fprintf(output, "}\n");

	fflush(output);
}
