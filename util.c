#include <stdint.h>
#include <stdio.h>

#include "util.h"
#include "nfa_types.h"

const char * epsilon_utf8 = "\xCE\xB5";

const char * char_to_string(int ch)
{
	static char buffer[12];
	if(ch == EMPTY_CHAR)
		return epsilon_utf8;
	sprintf(buffer, "%c", (char)ch);
	return buffer;
}

