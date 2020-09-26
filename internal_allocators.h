#ifndef FSMCONV_INTERNAL_ALLOCATORS_H
#define FSMCONV_INTERNAL_ALLOCATORS_H 1

#define INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE (8192)

void * alloc_bytes(unsigned int nbytes);

void drop_internal_allocators(void);


#endif /* FSMCONV_INTERNAL_ALLOCATORS_H */
