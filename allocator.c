#include "allocator.h"

#include <stdio.h>
#include <stdlib.h>

void * allocate(struct allocator * allocator, unsigned int nbytes)
{
	struct allocator_chunk_entry * chunk;
	unsigned int alignment;

	chunk = allocator->chunks;
	alignment = allocator->alignment;

	nbytes = (nbytes + alignment - 1) & ~(alignment - 1);

	if(nbytes > allocator->chunk_size) {
		fprintf(stderr, "error: tries to allocate too many bytes for '%s' allocator!\n", allocator->name);
		exit(EXIT_FAILURE);
	}

	if(!chunk || nbytes > chunk->remains) {
		struct allocator_chunk_entry * new_chunk = (struct allocator_chunk_entry *) malloc(allocator->chunk_size);
		if(!new_chunk) {
			fprintf(stderr, "error: out of memory!\n");
			exit(EXIT_FAILURE);
		}
		size_t struct_member_offset = (size_t) &((struct allocator_chunk_entry *) 0)->memory;
		unsigned int offset = ((unsigned int) struct_member_offset + alignment - 1) & ~(alignment - 1);
		new_chunk->offset = offset - struct_member_offset;
		new_chunk->remains = allocator->chunk_size - offset;
		new_chunk->next = allocator->chunks;
		allocator->chunks = new_chunk;

		chunk = new_chunk;
	}

	void * memory = (void *)chunk->memory + chunk->offset;
	chunk->offset += nbytes;
	chunk->remains -= nbytes;
	return memory;
}

void deallocate(struct allocator * allocator)
{
	struct allocator_chunk_entry * chunk, * next;

	chunk = allocator->chunks;

	allocator->chunks = NULL;
	allocator->free_objects = NULL;

	while(chunk) {
		next = chunk->next;
		free((void *) chunk);
		chunk = next;
	}
}

void * alloc_object(struct allocator * allocator, unsigned int size)
{
	if(allocator->free_objects) {
		void ** entry = (void **) allocator->free_objects;
		allocator->free_objects = *entry;
		return (void *) entry;
	}
	return allocate(allocator, size);
}

void free_object(struct allocator * allocator, void * object)
{
	void ** entry = (void **) object;
	*entry = allocator->free_objects;
	allocator->free_objects = (void *) entry;
}
