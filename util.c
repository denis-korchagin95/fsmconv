#include <stdint.h>
#include <stdio.h>

#include "util.h"
#include "nfa_types.h"

const char * char_to_string(int ch)
{
  static char buffer[4];
  if(ch == EMPTY_CHAR)
	  return "<empty>";
  else
    sprintf(buffer, "%c", (char)ch);
  return buffer;
}

