#ifndef NFA2DFACONV_ALLOCATOR_H
#define NFA2DFACONV_ALLOCATOR_H 1

#define define_allocator(name, type)      \
  extern type * ___alloc_ ## name(void);  \
  extern uint32_t name ## _pool_cursor;   \
  extern type name ## _pool[];            \

#define declare_allocator(name, type, count)                        \
  type name ## _pool[(count)];                                      \
  uint32_t name ## _pool_cursor = 0;                                \
                                                                    \
  type * ___alloc_ ## name (void)                                   \
  {                                                                 \
    if(name ## _pool_cursor >= (count)) {                           \
      fprintf(stderr, ("___alloc_" #name ": out of memory!\n"));    \
      exit(1);                                                      \
    }                                                               \
    return &name ## _pool[name ## _pool_cursor++];                  \
  }                                                                 \


define_allocator(nfa_state, struct nfa_state);
define_allocator(nfa_transition, struct nfa_transition);
define_allocator(nfa, struct nfa);
define_allocator(nfa_state_set, struct nfa_state_set);
define_allocator(nfa_char_set, struct nfa_char_set);
define_allocator(nfa_state_list, struct nfa_state_list);

define_allocator(token, struct token);
define_allocator(symbol, struct symbol);
define_allocator(identifier, struct identifier);

void show_allocation_stats(FILE * output);


#endif /* NFA2DFACONV_ALLOCATOR_H */
