#include "allocator.h"


declare_allocator(fsm_state, struct fsm_state, 1024)
declare_allocator(fsm, struct fsm, 16)
declare_allocator(fsm_transition, struct fsm_transition, 2048)
declare_allocator(fsm_state_list, struct fsm_state_list, 4096)
declare_allocator(character_list, struct character_list, 128)

declare_allocator(token, struct token, 1024)
declare_allocator(symbol, struct symbol, 1024)
declare_allocator(identifier, struct identifier, 1024)

void show_allocation_stats(FILE * output)
{
  size_t total_bytes = 0;
  size_t total_allocation_unit_count = 0;
  size_t current_bytes;

  current_bytes = sizeof(struct fsm_state) * fsm_state_pool_cursor;
  fprintf(output, "Allocated fsm_state elements: %d ( mem: %lu bytes )\n", fsm_state_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_state_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct fsm) * fsm_pool_cursor;
  fprintf(output, "Allocated fsm elements: %d ( mem: %lu bytes )\n", fsm_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct fsm_transition) * fsm_transition_pool_cursor;
  fprintf(output, "Allocated fsm_transition elements: %d ( mem: %lu bytes )\n", fsm_transition_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_transition_pool_cursor;
  total_bytes += current_bytes;

  current_bytes = sizeof(struct fsm_state_list) * fsm_state_list_pool_cursor;
  fprintf(output, "Allocated fsm_state_list elements: %d ( mem: %lu bytes )\n", fsm_state_list_pool_cursor, current_bytes);
  total_allocation_unit_count += fsm_state_list_pool_cursor;
  total_bytes += current_bytes;
  
  current_bytes = sizeof(struct character_list) * character_list_pool_cursor;
  fprintf(output, "Allocated character_list elements: %d ( mem: %lu bytes )\n", character_list_pool_cursor, current_bytes);
  total_allocation_unit_count += character_list_pool_cursor;
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
