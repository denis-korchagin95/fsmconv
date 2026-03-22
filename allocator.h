#ifndef FSMCONV_ALLOCATOR_H
#define FSMCONV_ALLOCATOR_H 1

struct area_block
{
	struct area_block * next;
	unsigned int used;
	char memory[];
};

struct area
{
	struct area_block * blocks;
	unsigned int block_size;
};

extern struct area area;

extern void * area_alloc(struct area * area, unsigned int nbytes);
extern void area_free(struct area * area);
extern void area_cleanup(void);

#define alloc(type) ((type *) area_alloc(&area, sizeof(type)))
#define alloc_bytes(nbytes) (area_alloc(&area, (nbytes)))

#endif /* FSMCONV_ALLOCATOR_H */
