#include "character_list.h"
#include "util.h"

bool character_list_has_character(struct character_list * characters, int ch)
{
    struct character_list * it;
    list_foreach(it, characters) {
        if (it->ch == ch)
            return true;
    }
    return false;
}
