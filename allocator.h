#ifndef FSMCONV_ALLOCATOR_H
#define FSMCONV_ALLOCATOR_H 1

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "fsm_types.h"
#include "parser_types.h"
#include "character_list.h"


struct allocator_chunk_entry
{
	struct allocator_chunk_entry * next;
	unsigned int offset;
	unsigned int remains;
	char memory[];
};

struct allocator
{
	const char * name;
	struct allocator_chunk_entry * chunks;
	void * free_objects;
	unsigned int alignment;
	unsigned int chunk_size;
};

extern void * allocate(struct allocator * allocator, unsigned int nbytes);
extern void deallocate(struct allocator * allocator);

extern void * alloc_object(struct allocator * allocator, unsigned int size);

extern void free_object(struct allocator * allocator, void * object);

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
