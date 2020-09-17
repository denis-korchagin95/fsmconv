#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "fsm_types.h"
#include "util.h"
#include "visualize.h"
#include "fsm.h"

void visualize_nfa(FILE * output, struct fsm * fsm)
{
	struct fsm_state * state;
	struct fsm_transition * transition;
	struct fsm_state_list * state_list;
	bool is_initial, is_finished;

	fprintf(output, "digraph nfa {\n");

	state = fsm->states;

	while(state != NULL) {
		is_initial = state->attrs & FSM_STATE_ATTR_INITIAL;
		is_finished = state->attrs & FSM_STATE_ATTR_FINISHED;

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
				fprintf(output, "\ts%d -> s%d [label=\"%s\"];\n", state->id, state_list->state_id, char_to_string(transition->ch));
				state_list = state_list->next;
			}
			transition = transition->next;
		}

		state = state->next;
	}

	fprintf(output, "}\n");

	fflush(output);
}

void generate_nfa_language(FILE * output, struct fsm * fsm)
{
    struct fsm_state * state;

    fprintf(output, "start ");
    bool is_first = true;
    list_foreach(state, fsm->states) {
        if (state->attrs & FSM_STATE_ATTR_INITIAL) {
            if (is_first) {
                fprintf(output, "%s", state->name);
                is_first = false;
                continue;
            }
            fprintf(output, ", %s", state->name);
        }
    }
    fprintf(output, ";\n");

    fprintf(output, "end ");
    is_first = true;
    list_foreach(state, fsm->states) {
        if (state->attrs & FSM_STATE_ATTR_FINISHED) {
            if (is_first) {
                fprintf(output, "%s", state->name);
                is_first = false;
                continue;
            }
            fprintf(output, ", %s", state->name);
        }
    }
    fprintf(output, ";\n");

    fprintf(output, "\n");

    struct fsm_transition * transition;
    struct fsm_state_list * state_item;
    struct fsm_state * target_state;

    list_foreach(state, fsm->states) {
        list_foreach(transition, state->transitions) {
            list_foreach(state_item, transition->states) {
                target_state = fsm_search_state_by_id(fsm, state_item->state_id);
                if (target_state == NULL)
                    continue;
                fprintf(output, "%s to %s by ", state->name, target_state->name);
                if (transition->ch == EMPTY_CHAR) {
                    fprintf(output, "@epsilon");
                }
                else {
                    fprintf(output, "%c", transition->ch);
                }
                fprintf(output, ";\n");
            }
        }
    }

    fflush(output);
}

void generate_dfa_language(FILE * output, struct fsm * fsm)
{
    struct fsm_state * state, * target_state;
    struct fsm_transition * transition;

    fprintf(output, "start ");
    bool is_first = true;
    list_foreach(state, fsm->states) {
        if (state->attrs & FSM_STATE_ATTR_INITIAL) {
            if (is_first) {
                fprintf(output, "s%u", state->id);
                is_first = false;
                continue;
            }
            fprintf(output, ", s%u", state->id);
        }
    }
    fprintf(output, ";\n");

    fprintf(output, "end ");
    is_first = true;
    list_foreach(state, fsm->states) {
        if (state->attrs & FSM_STATE_ATTR_FINISHED) {
            if (is_first) {
                fprintf(output, "s%u", state->id);
                is_first = false;
                continue;
            }
            fprintf(output, ", s%u", state->id);
        }
    }
    fprintf(output, ";\n");

    fprintf(output, "\n");

    list_foreach(state, fsm->states) {
        list_foreach(transition, state->transitions) {
            target_state = fsm_search_state_by_id(fsm, transition->states->state_id);
            if (target_state == NULL)
                continue;
            fprintf(output, "s%u to s%u by %c;\n", state->id, target_state->id, transition->ch);
        }
    }

    fflush(output);
}
