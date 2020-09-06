#include "allocator.h"

declare_allocator(nfa_state, struct nfa_state, 1024)
declare_allocator(nfa_transition, struct nfa_transition, 2048)
declare_allocator(nfa, struct nfa, 16)
declare_allocator(nfa_state_set, struct nfa_state_set, 4096)
declare_allocator(nfa_char_set, struct nfa_char_set, 128)

declare_allocator(token, struct token, 1024)
