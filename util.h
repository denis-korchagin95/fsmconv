#ifndef FSMCONV_UTIL_H
#define FSMCONV_UTIL_H 1

#include <stddef.h>
#include <stdbool.h>

#if _WIN32
    #define DIRECTORY_SEPARATOR '\\'
#else
    #define DIRECTORY_SEPARATOR '/'
#endif

#define TAB_SIZE (8)

#define bit(n) (1u << (n))

#define list_foreach(loop_var, initial_value) for((loop_var) = (initial_value); (loop_var) != NULL; (loop_var) = (loop_var)->next)

extern const char * epsilon_utf8;

extern const char * char_to_string(int ch);

extern void u8_bits_set(unsigned char * bits, unsigned int nbits, unsigned int bit);
extern void u8_bits_unset(unsigned char * bits, unsigned int nbits, unsigned int bit);
extern bool u8_bits_check(unsigned char * bits, unsigned int nbits, unsigned int bit);

#endif /* FSMCONV_UTIL_H */
