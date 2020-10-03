#include "symbol.h"
#include "tokenizer.h"

struct symbol * lookup_symbol(struct identifier * identifier, int type)
{
	struct symbol * it = identifier->symbols;
	while(it != NULL) {
		if(it->type == type)
			return it;
		it = it->next;
	}
	return NULL;
}
