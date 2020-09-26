#include "internal_allocators.h"
#include "allocator.h"

static struct allocator bytes_allocator = {
	"bytes", NULL, NULL, __alignof__(void), INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE
};

void * alloc_bytes(unsigned int nbytes)
{
	return allocate(&bytes_allocator, nbytes);
}

void drop_internal_allocators(void)
{
	deallocate(&bytes_allocator);
}
