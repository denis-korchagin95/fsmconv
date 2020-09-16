#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "util.h"
#include "nfa_types.h"
#include "nfa_character_list.h"


bool nfa_character_list_has_character(struct nfa_character_list * characters, int ch)
{
    struct nfa_character_list * it;
    list_foreach(it, characters) {
        if (it->ch == ch)
            return true;
    }
    return false;
}
