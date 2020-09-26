#include "internal_allocators.h"

static struct allocator bytes_allocator = {
	"bytes", NULL, NULL, __alignof__(void), INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE
};

void * alloc_bytes(unsigned int nbytes)
{
	return allocate(&bytes_allocator, nbytes);
}

internal_object_allocator_declare(identifier, struct identifier, identifiers, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)

void drop_internal_allocators(void)
{
	deallocate(&bytes_allocator);
	deallocate(&identifiers_allocator);
}
