#include "allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define AREA_DEFAULT_BLOCK_SIZE (8192)

struct area area = { NULL, AREA_DEFAULT_BLOCK_SIZE };

void * area_alloc(struct area * area, unsigned int nbytes)
{
	struct area_block * block;
	unsigned int capacity;

	capacity = area->block_size - (unsigned int) offsetof(struct area_block, memory);

	if(nbytes > capacity) {
		fprintf(stderr, "error: area allocation too large!\n");
		exit(EXIT_FAILURE);
	}

	block = area->blocks;

	if(!block || block->used + nbytes > capacity) {
		block = (struct area_block *) malloc(area->block_size);
		if(!block) {
			fprintf(stderr, "error: out of memory!\n");
			exit(EXIT_FAILURE);
		}
		block->used = 0;
		block->next = area->blocks;
		area->blocks = block;
	}

	{
		void * memory = block->memory + block->used;
		block->used += nbytes;
		return memory;
	}
}

void area_cleanup(void)
{
	area_free(&area);
}

void area_free(struct area * area)
{
	struct area_block * block, * next;

	block = area->blocks;
	area->blocks = NULL;

	while(block) {
		next = block->next;
		free(block);
		block = next;
	}
}
