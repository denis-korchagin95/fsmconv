#ifndef NFA2DFA_CONV_UTIL_H
#define NFA2DFA_CONV_UTIL_H 1

#define bit(n) (1u << (n))

#define list_foreach(loop_var, initial_value) for((loop_var) = (initial_value); (loop_var) != NULL; (loop_var) = (loop_var)->next)

extern const char * epsilon_utf8;

const char * char_to_string(int ch);

#endif /* NFA2DFA_CONV_UTIL_H */
