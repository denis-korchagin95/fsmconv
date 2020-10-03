#ifndef FSMCONV_ALLOCATOR_H
#define FSMCONV_ALLOCATOR_H 1

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

#endif /* FSMCONV_ALLOCATOR_H */
