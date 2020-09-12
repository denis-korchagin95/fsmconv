#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "nfa_types.h"
#include "parser_types.h"
#include "allocator.h"

declare_allocator(nfa_state_set, struct nfa_state_set, 4096)
declare_allocator(nfa_state, struct nfa_state, 1024)
declare_allocator(nfa, struct nfa, 16)
declare_allocator(nfa_transition, struct nfa_transition, 2048)
declare_allocator(nfa_char_set, struct nfa_char_set, 128)
declare_allocator(nfa_state_list, struct nfa_state_list, 4096)

declare_allocator(token, struct token, 1024)
declare_allocator(symbol, struct symbol, 1024)
declare_allocator(identifier, struct identifier, 1024)

void show_allocation_stats(FILE * output)
{
  size_t total_bytes = 0;
  size_t total_allocation_unit_count = 0;
  size_t current_bytes;

  current_bytes = sizeof(struct nfa_state_set) * nfa_state_set_pool_cursor;
  fprintf(output, "\n\n\nAllocated nfa_state_set elements: %d ( mem: %lu bytes )\n", nfa_state_set_pool_cursor, current_bytes);
  total_allocation_unit_count += nfa_state_set_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct nfa_state) * nfa_state_pool_cursor;
  fprintf(output, "Allocated nfa_state elements: %d ( mem: %lu bytes )\n", nfa_state_pool_cursor, current_bytes);
  total_allocation_unit_count += nfa_state_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct nfa) * nfa_pool_cursor;
  fprintf(output, "Allocated nfa elements: %d ( mem: %lu bytes )\n", nfa_pool_cursor, current_bytes);
  total_allocation_unit_count += nfa_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct nfa_transition) * nfa_transition_pool_cursor;
  fprintf(output, "Allocated nfa_transition elements: %d ( mem: %lu bytes )\n", nfa_transition_pool_cursor, current_bytes);
  total_allocation_unit_count += nfa_transition_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct nfa_char_set) * nfa_char_set_pool_cursor;
  fprintf(output, "Allocated nfa_char_set elements: %d ( mem: %lu bytes )\n", nfa_char_set_pool_cursor, current_bytes);
  total_allocation_unit_count += nfa_char_set_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct token) * token_pool_cursor;
  fprintf(output, "Allocated token elements: %d ( mem: %lu bytes )\n", token_pool_cursor, current_bytes);
  total_allocation_unit_count += token_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct symbol) * symbol_pool_cursor;
  fprintf(output, "Allocated symbol elements: %d ( mem: %lu bytes )\n", symbol_pool_cursor, current_bytes);
  total_allocation_unit_count += symbol_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct identifier) * identifier_pool_cursor;
  fprintf(output, "Allocated identifier elements: %d ( mem: %lu bytes )\n", identifier_pool_cursor, current_bytes);
  total_allocation_unit_count += identifier_pool_cursor;
  total_bytes += current_bytes;

  fprintf(output, "\n\nTotal allocation unit count: %lu\n", total_allocation_unit_count);
  fprintf(output, "Total bytes: %lu\n", total_bytes);
}
