#ifndef NFA2DFACONV_DEBUG_H
#define NFA2DFACONV_DEBUG_H 1

#include "parser_types.h"
#include <stdio.h>

void debug_token(FILE * output, struct token * token);
void debug_symbol(FILE * output, struct symbol * symbol);

#endif /* NFA2DFACONV_DEBUG_H */
