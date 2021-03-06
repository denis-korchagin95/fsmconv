#ifndef FSMCONV_CHARACTER_LIST_H
#define FSMCONV_CHARACTER_LIST_H 1

#include <stdbool.h>

struct character_list
{
	struct character_list * next;
	int ch;
};

extern bool character_list_has_character(struct character_list * characters, int ch);

#endif /* FSMCONV_CHARACTER_LIST_H */
