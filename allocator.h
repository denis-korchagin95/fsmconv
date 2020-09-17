#ifndef FSMCONV_ALLOCATOR_H
#define FSMCONV_ALLOCATOR_H 1

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


define_allocator(fsm_state, struct fsm_state);
define_allocator(fsm_transition, struct fsm_transition);
define_allocator(fsm, struct fsm);
define_allocator(fsm_state_list, struct fsm_state_list);

define_allocator(character_list, struct character_list);

define_allocator(token, struct token);
define_allocator(symbol, struct symbol);
define_allocator(identifier, struct identifier);

void show_allocation_stats(FILE * output);


#endif /* FSMCONV_ALLOCATOR_H */
