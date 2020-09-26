#include "allocator.h"

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

declare_allocator(fsm_state, struct fsm_state, 1024)
declare_allocator(fsm, struct fsm, 16)
declare_allocator(fsm_transition, struct fsm_transition, 2048)
declare_allocator(fsm_state_list, struct fsm_state_list, 4096)
declare_allocator(character_list, struct character_list, 128)

declare_allocator(token, struct token, 1024)
declare_allocator(symbol, struct symbol, 1024)

void show_allocation_stats(FILE * output)
{
  size_t total_bytes = 0;
  size_t total_allocation_unit_count = 0;
  size_t current_bytes;

  current_bytes = sizeof(struct fsm_state) * fsm_state_pool_cursor;
  fprintf(output, "Allocated fsm_state elements: %d ( mem: %lu bytes )\n", fsm_state_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_state_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct fsm) * fsm_pool_cursor;
  fprintf(output, "Allocated fsm elements: %d ( mem: %lu bytes )\n", fsm_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct fsm_transition) * fsm_transition_pool_cursor;
  fprintf(output, "Allocated fsm_transition elements: %d ( mem: %lu bytes )\n", fsm_transition_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_transition_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct fsm_state_list) * fsm_state_list_pool_cursor;
  fprintf(output, "Allocated fsm_state_list elements: %d ( mem: %lu bytes )\n", fsm_state_list_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_state_list_pool_cursor;
  total_bytes += current_bytes;
  
  current_bytes = sizeof(struct character_list) * character_list_pool_cursor;
  fprintf(output, "Allocated character_list elements: %d ( mem: %lu bytes )\n", character_list_pool_cursor, current_bytes);
  total_allocation_unit_count += character_list_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct token) * token_pool_cursor;
  fprintf(output, "Allocated token elements: %d ( mem: %lu bytes )\n", token_pool_cursor, current_bytes);
  total_allocation_unit_count += token_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct symbol) * symbol_pool_cursor;
  fprintf(output, "Allocated symbol elements: %d ( mem: %lu bytes )\n", symbol_pool_cursor, current_bytes);
  total_allocation_unit_count += symbol_pool_cursor;
  total_bytes += current_bytes;

  fprintf(output, "\n\nTotal allocation unit count: %lu\n", total_allocation_unit_count);
  fprintf(output, "Total bytes: %lu\n", total_bytes);
}
