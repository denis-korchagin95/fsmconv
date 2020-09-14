#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#include "util.h"
#include "parser.h"
#include "nfa_types.h"
#include "parser_types.h"
#include "debug.h"
#include "allocator.h"
#include "nfa_compiler.h"
#include "visualize.h"

static const char * filename = "./examples/ip4_nfa.txt";
static const char * nfa_graph_filename = "./nfa.dot";
static const char * dfa_graph_filename = "./dfa.dot";

#define CHARACTERS_COUNT 128

#define add_state(nfa, state)           \
  do                                    \
  {                                     \
    *(nfa)->last_state = (state);       \
    (nfa)->last_state = &(state)->next; \
  }                                     \
  while(0)


/*
static bool nfa_state_set_has_element(struct nfa_state_set * set, uint32_t element)
{
  struct nfa_state_set * iterator = set;
  while(iterator)
  {
    if(iterator->element == element)
      return true;
    iterator = iterator->next;
  }
  return false;
}

static struct nfa_state * nfa_find_not_visited_state(struct nfa * nfa)
{
  struct nfa_state * iterator = nfa->states;
  while(iterator) {
    if(! (iterator->attrs & NFA_STATE_ATTR_VISITED))
      return iterator;
    iterator = iterator->next;
  }
  return NULL;
}


static struct nfa * nfa_create(void)
{
  struct nfa * nfa = ___alloc_nfa();
  nfa->states = NULL;
  nfa->last_state = &nfa->states;
  return nfa;
}


static struct nfa_char_set * nfa_char_set_create(int ch)
{
  struct nfa_char_set * el = ___alloc_nfa_char_set();
  el->ch = ch;
  el->next = NULL;
  return el;
}


static struct nfa_state_set * nfa_state_set_create(uint32_t element)
{
  struct nfa_state_set * el = ___alloc_nfa_state_set();
  el->element = element;
  el->next = NULL;
  return el;
}

static struct nfa_state * nfa_state_create(void)
{
  struct nfa_state * el = ___alloc_nfa_state();
  el->attrs = 0;
  el->id = 0;
  el->transitions = NULL;
  el->subset = NULL;
  el->next = NULL;
  return el;
}


static struct nfa_state_set * nfa_state_set_copy(struct nfa_state_set * states)
{
  struct nfa_state_set * set = NULL, * el, * iterator = states;
  while(iterator) {
    el = nfa_state_set_create(iterator->element);
    el->next = set;
    set = el;
    iterator = iterator->next;
  }
  return set;
}

static struct nfa_transition * nfa_transition_copy(struct nfa_transition * el)
{
  struct nfa_transition * new_el = ___alloc_nfa_transition();
  new_el->states = nfa_state_set_copy(el->states);
  new_el->ch = el->ch;
  new_el->next = NULL;
  return new_el;
}

static struct nfa_transition * nfa_transitions_copy(struct nfa_transition * list)
{
  struct nfa_transition * transitions = NULL, * iterator = list, * el;
  while(iterator) {
    el = nfa_transition_copy(iterator);
    el->next = transitions;
    transitions = el;
    iterator = iterator->next;
  }
  return transitions;
}

static struct nfa_state * nfa_state_copy(struct nfa_state * state)
{
  struct nfa_state * el = ___alloc_nfa_state();
  el->id = state->id;
  el->attrs = state->attrs;
  el->subset = nfa_state_set_copy(state->subset);
  el->transitions = nfa_transitions_copy(state->transitions);
  el->next = NULL;
  return el;
}


static struct nfa_transition * nfa_get_transition(struct nfa_state * state, int ch)
{
  if(!state)
    goto ret;
  struct nfa_transition * iterator = state->transitions;
  while(iterator)
  {
    if(iterator->ch == ch)
      return iterator;
    iterator = iterator->next;
  }
ret:
  return NULL;
}

static int nfa_state_set_find_state(struct nfa_state_set * states, uint32_t state_id)
{
  struct nfa_state_set * ss_iter = states;
  while(ss_iter)
  {
    if(ss_iter->element == state_id)
      return 0;
    ss_iter = ss_iter->next;
  }
  return -1;
}



static struct nfa_transition * nfa_transition_create(int ch, struct nfa_state_set * states)
{
  struct nfa_transition * el = ___alloc_nfa_transition();
  el->states = states;
  el->ch = ch;
  el->next = NULL;
  return el;
}

static struct nfa_transition * nfa_state_add_transition(struct nfa_state * state, int ch, uint32_t target)
{
  struct nfa_transition * transition = nfa_get_transition(state, ch);

  struct nfa_state_set * el = nfa_state_set_create(target);

  if(!transition) {
    transition = nfa_transition_create(ch, el);
    transition->next = state->transitions;
    state->transitions = transition;
  }
  else if(nfa_state_set_find_state(transition->states, target) == -1) {
    el->next = transition->states;
    transition->states = el;
  }

  return transition;
}
*/

/*
static struct nfa_state * nfa_get_state_by_id(struct nfa_state * states, uint32_t id)
{
  struct nfa_state * iterator = states;
  while(iterator) {
    if(iterator->id == id)
      return iterator;
    iterator = iterator->next;
  }
  return NULL;
}

static size_t nfa_state_set_nitems(struct nfa_state_set * set)
{
  size_t nitems;
  struct nfa_state_set * iterator;
  for(iterator = set, nitems = 0; iterator; iterator = iterator->next, ++nitems)
    ;
  return nitems;
}

static bool nfa_state_set_contains(struct nfa_state_set * set0, struct nfa_state_set * set1)
{
  bool is_contains = true;
  struct nfa_state_set * iterator;
  for(iterator = set1; iterator; iterator = iterator->next)
    if(! nfa_state_set_has_element(set0, iterator->element))
    {
      is_contains = false;
      break;
    }
  return is_contains;
}

static bool nfa_state_set_equals(struct nfa_state_set * set0, struct nfa_state_set * set1)
{
  return nfa_state_set_nitems(set0) == nfa_state_set_nitems(set1) && nfa_state_set_contains(set0, set1);
}

static struct nfa_state * nfa_find_state_subset(struct nfa * nfa, struct nfa_state_set * subset)
{
  if(!subset)
    goto ret;
  struct nfa_state * iterator = nfa->states;
  while(iterator) {
    if(iterator->subset && nfa_state_set_equals(iterator->subset, subset))
      return iterator;
    iterator = iterator->next;
  }
ret:
  return NULL;
}
*/

/*
static struct nfa_transition * nfa_transition_subset_update(struct nfa_state * state, int ch, struct nfa_state_set * subset)
{
  struct nfa_transition * transition = nfa_get_transition(state, ch);
  if(!transition) {
    transition = nfa_transition_create(ch, subset);
    transition->next = state->transitions;
    state->transitions = transition;
  }
  else {
  */
    /* TODO: destroy old subset */
/*
    transition->states = subset;
  }
  return transition;
}

static void print_state_set(struct nfa_state_set * states)
{
  while(states) {
    printf(" %u", states->element);
    states = states->next;
  }
}

static void print_transition(struct nfa_transition * transition)
{
  if(!transition)
  {
    printf("%p\n", transition);
    return;
  }
  printf("states:");
  print_state_set(transition->states);
  printf("\nch: %c\n", transition->ch);
}

static void nfa_state_set_concat(struct nfa_state_set ** dest, struct nfa_state_set * source)
{
  struct nfa_state_set * new, * iterator = source;
  while(iterator) {
    new = nfa_state_set_create(iterator->element);
    new->next = *dest;
    *dest = new;
    iterator = iterator->next;
  }
}

static bool nfa_char_set_has_char(struct nfa_char_set * chars, int ch)
{
  struct nfa_char_set * iterator = chars;
  while(iterator) {
    if(iterator->ch == ch)
      return true;
    iterator = iterator->next;
  }
  return false;
}

static struct nfa_state_set * nfa_empty_closure(struct nfa_state * nfa_states, struct nfa_state * dfa_states, struct nfa_state_set * set)
{
  struct nfa_state_set * empty_closure = nfa_state_set_copy(set);
  struct nfa_state_set * stack = nfa_state_set_copy(set);
  struct nfa_state_set * hold, * current, * ss_iter;
  struct nfa_state * state;
  struct nfa_transition * transition;

  while(stack)
  {
    hold = stack->next;
    current = stack;
    stack = hold;

    state = nfa_get_state_by_id(nfa_states, current->element);
    if(!state)
      state = nfa_get_state_by_id(dfa_states, current->element);

    if(!state)
      continue;

    transition = nfa_get_transition(state, EMPTY_CHAR);

    if(!transition)
      continue;

    ss_iter = transition->states;

    while(ss_iter) {
      if(! nfa_state_set_has_element(empty_closure, ss_iter->element))
      {
        hold = nfa_state_set_create(ss_iter->element);
        hold->next = empty_closure;
        empty_closure = hold;

        hold = nfa_state_set_create(ss_iter->element);
        hold->next = stack;
        stack = hold;
      }
      ss_iter = ss_iter->next;
    }
  }

  return empty_closure;
}
*/

int main(int argc, char * argv[])
{
  init_parser();

  if (argc < 2) {
      fprintf(stderr, "%s: missing operand\n", argv[0]);
      exit(1);
  }

  FILE * file = fopen(filename, "r");

  struct symbol * parse_tree = parse(file);

  fclose(file);

  if (parse_tree == NULL) {
	  printf("parse file error!\n");
	  exit(1);
  }

  struct nfa * nfa;

  nfa = nfa_compile(parse_tree);

  FILE * output = fopen(nfa_graph_filename, "w");
  if(!output) {
    fprintf(stderr, "file '%s': %s\n", nfa_graph_filename, strerror(errno));
    exit(1);
  }

  visualize_nfa(nfa, output);

  fclose(output);

  /*
  debug_symbol(stdout, parse_tree, 0);
  puts("");
  */

  show_allocation_stats(stdout);

  exit(0);

  /*
  FILE * output = fopen(nfa_graph_filename, "w");
  if(!output) {
    fprintf(stderr, "file '%s': %s\n", nfa_graph_filename, strerror(errno));
    exit(1);
  }

  visualize_nfa(nfa, output);

  fclose(output);
  */

  /* nfa to dfa conversion ( without e-transitions ) */
  /*
  struct nfa_state_set * initial_states = NULL;
  struct nfa_state_set * finished_states = NULL;
  struct nfa_state * state, * new_state;
  struct nfa_state_set * ss_tmp, * ss_iter;
  struct nfa_state_set * subset;
  struct nfa_transition * transition;
  struct nfa * dfa;
  struct nfa_char_set * chars = NULL, * new_char, * char_iter;

  dfa = nfa_create();
  */

  /* collect extra information for algorithm */
  /*
  state = nfa->states;
  while(state) {
    if(state->attrs & NFA_STATE_ATTR_INITIAL)
    {
      new_state = nfa_state_copy(state);
      new_state->subset = nfa_empty_closure(nfa->states, NULL, nfa_state_set_create(new_state->id));
      add_state(dfa, new_state);

      ss_tmp = nfa_state_set_create(state->id);
      ss_tmp->next = initial_states;
      initial_states = ss_tmp;
    }
    if(state->attrs & NFA_STATE_ATTR_FINISHED) {
      ss_tmp = nfa_state_set_create(state->id);
      ss_tmp->next = finished_states;
      finished_states = ss_tmp;
    }
    transition = state->transitions;
    while(transition) {
      if(transition->ch != EMPTY_CHAR && !nfa_char_set_has_char(chars, transition->ch))
      {
        new_char = nfa_char_set_create(transition->ch);
        new_char->next = chars;
        chars = new_char;
      }
      transition = transition->next;
    }
    state = state->next;
  }

  while((state = nfa_find_not_visited_state(dfa))) {
    state->attrs |= NFA_STATE_ATTR_VISITED;

    char_iter = chars;

    while(char_iter) {
      subset = NULL;

      ss_iter = state->subset;
      while(ss_iter) {
        new_state = nfa_get_state_by_id(nfa->states, ss_iter->element);
        if(!new_state)
          new_state = nfa_get_state_by_id(dfa->states, ss_iter->element);
        transition = nfa_get_transition(new_state, char_iter->ch);
        if(transition && transition->states)
          nfa_state_set_concat(&subset, transition->states);
        ss_iter = ss_iter->next;
      }

      if(!subset)
        goto next_char;

      subset = nfa_empty_closure(nfa->states, dfa->states, subset);

      new_state = nfa_find_state_subset(dfa, subset);

      if(!new_state)
      {
        new_state = nfa_state_create();

        if(nfa_state_set_contains(initial_states, subset))
          new_state->attrs |= NFA_STATE_ATTR_INITIAL;

        for(ss_iter = subset; ss_iter; ss_iter = ss_iter->next)
          if(nfa_state_set_has_element(finished_states, ss_iter->element))
          {
            new_state->attrs |= NFA_STATE_ATTR_FINISHED;
            break;
          }

        new_state->subset = subset;

        add_state(dfa, new_state);
      }

      nfa_transition_subset_update(state, char_iter->ch, nfa_state_set_create(new_state->id));

next_char:
      char_iter = char_iter->next;
    }
  }

  state = dfa->states;
  struct nfa_transition ** t_hold;
  bool was_empty;
  while(state)
  {
    t_hold = &state->transitions;
    was_empty = false;
    while((*t_hold)) {
      if((*t_hold)->ch == EMPTY_CHAR)
      {
        was_empty = true;
        break;
      }
      t_hold = &(*t_hold)->next;
    }
    if(was_empty && *t_hold)
      *t_hold = (*t_hold)->next;
    state = state->next;
  }

  output = fopen(dfa_graph_filename, "w");
  if(!output) {
    fprintf(stderr, "file '%s': %s\n", dfa_graph_filename, strerror(errno));
    exit(1);
  }

  visualize_nfa(dfa, output);

  fclose(output);
  */

  return 0;
}
