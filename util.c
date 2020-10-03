#include "util.h"
#include "fsm_types.h"

#include <stdio.h>
#include <limits.h>

const char * epsilon_utf8 = "\xCE\xB5";

const char * char_to_string(int ch)
{
	static char buffer[12];
	if(ch == EPSILON_CHAR)
		return epsilon_utf8;
	sprintf(buffer, "%c", (char)ch);
	return buffer;
}

void u8_bits_set(unsigned char * bits, unsigned int nbits, unsigned int bit)
{
	unsigned int byte_index = bit / CHAR_BIT;
	unsigned int bit_offset = bit - byte_index * CHAR_BIT;
	*(bits + byte_index) |= (1 << (CHAR_BIT - 1 - bit_offset));
}

void u8_bits_unset(unsigned char * bits, unsigned int nbits, unsigned int bit)
{
	unsigned int byte_index = bit / CHAR_BIT;
	unsigned int bit_offset = bit - byte_index * CHAR_BIT;
	*(bits + byte_index) &= ~(1 << (CHAR_BIT - 1 - bit_offset));
}

bool u8_bits_check(unsigned char * bits, unsigned int nbits, unsigned int bit)
{
	unsigned int byte_index = bit / CHAR_BIT;
	unsigned int bit_offset = bit - byte_index * CHAR_BIT;
	return *(bits + byte_index) & (1 << (CHAR_BIT - 1 - bit_offset));
}
