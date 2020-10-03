#include "internal_allocators.h"
#include "tokenizer.h"
#include "ast.h"
#include "symbol.h"
#include "fsm_types.h"
#include "character_list.h"

static struct allocator bytes_allocator = {
	"bytes", NULL, NULL, __alignof__(void), INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE
};

void * alloc_bytes(unsigned int nbytes)
{
	return allocate(&bytes_allocator, nbytes);
}

internal_object_allocator_declare(identifier, struct identifier, identifiers, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(token, struct token, tokens, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(ast, struct ast, ast_nodes, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(symbol, struct symbol, symbols, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(ast_list, struct ast_list, ast_list, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)

internal_object_allocator_declare(fsm_state_list, struct fsm_state_list, fsm_state_list, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(fsm_transition, struct fsm_transition, fsm_transition, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(fsm, struct fsm, fsm, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(fsm_state, struct fsm_state, fsm_state, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)
internal_object_allocator_declare(character_list, struct character_list, character_list, INTERNAL_ALLOCATOR_DEFAULT_CHUNK_SIZE)

void drop_internal_allocators(void)
{
	deallocate(&bytes_allocator);

	deallocate(&identifiers_allocator);
	deallocate(&tokens_allocator);
	deallocate(&ast_nodes_allocator);
	deallocate(&symbols_allocator);
	deallocate(&ast_list_allocator);
	
	deallocate(&fsm_state_list_allocator);
	deallocate(&fsm_transition_allocator);
	deallocate(&fsm_allocator);
	deallocate(&fsm_state_allocator);
	deallocate(&character_list_allocator);
}
