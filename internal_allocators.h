#ifndef FSMCONV_INTERNAL_ALLOCATORS_H
#define FSMCONV_INTERNAL_ALLOCATORS_H 1

#include "allocator.h"

#define INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE (8192)

#define internal_object_allocator_define(identifier, type) \
	extern type * alloc_ ## identifier(void);              \
	extern void free_ ## identifier(type * object);        \

#define internal_object_allocator_declare(identifier, type, name, chunk_size) \
	static struct allocator name ## _allocator = {                            \
		#name, NULL, NULL, __alignof__(type), chunk_size                      \
	};                                                                        \
	type * alloc_ ## identifier(void)                                         \
	{                                                                         \
		return (type *) alloc_object(&name ## _allocator, sizeof(type));      \
	}                                                                         \
	void free_ ## identifier(type * object)                                   \
	{                                                                         \
		free_object(&name ## _allocator, (void *) object);                    \
	}                                                                         \

struct identifier;
struct token;
struct ast;
struct symbol;
struct ast_list;
struct fsm_state_list;
struct fsm_transition;
struct fsm;
struct fsm_state;
struct character_list;

extern void * alloc_bytes(unsigned int nbytes);

internal_object_allocator_define(identifier, struct identifier)
internal_object_allocator_define(token, struct token)
internal_object_allocator_define(ast, struct ast)
internal_object_allocator_define(symbol, struct symbol)
internal_object_allocator_define(ast_list, struct ast_list)

internal_object_allocator_define(fsm_state_list, struct fsm_state_list)
internal_object_allocator_define(fsm_transition, struct fsm_transition)
internal_object_allocator_define(fsm, struct fsm)
internal_object_allocator_define(fsm_state, struct fsm_state)
internal_object_allocator_define(character_list, struct character_list)

extern void drop_internal_allocators(void);

#endif /* FSMCONV_INTERNAL_ALLOCATORS_H */
