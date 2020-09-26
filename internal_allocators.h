#ifndef FSMCONV_INTERNAL_ALLOCATORS_H
#define FSMCONV_INTERNAL_ALLOCATORS_H 1

#include "allocator.h"

#define INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE (8192)

#define internal_object_allocator_define(identifier, type)  \
	type * alloc_ ## identifier(void);                      \
	void free_ ## identifier(type * object);                \

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

void * alloc_bytes(unsigned int nbytes);

internal_object_allocator_define(identifier, struct identifier);

void drop_internal_allocators(void);


#endif /* FSMCONV_INTERNAL_ALLOCATORS_H */
