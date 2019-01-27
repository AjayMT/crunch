
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "crunch.h"
#include "crunch_set.c"


static crunch_set_t runtime_memory_set;


void crunch_init()
{
  crunch_set_init(&runtime_memory_set);

  // FIXME
  --runtime_memory_set.count;
}


void crunch_finish()
{
  fprintf(stderr, "\n=== crunch ===\n");
  fprintf(stderr, "remaining heap allocations: %llu\n", runtime_memory_set.count);

  crunch_block_t *end = crunch_set_end(runtime_memory_set);
  crunch_block_t *begin = crunch_set_begin(runtime_memory_set);
  for (crunch_block_t *current = begin;
       current != end;
       current = crunch_set_next(runtime_memory_set, current))
    fprintf(stderr, "  %p : %lu bytes\n", (void *)(current->ptr), current->size);

  crunch_set_destroy(runtime_memory_set);
}


void *crunch_malloc(size_t size)
{
  void *ptr = malloc(size);
  crunch_set_put(&runtime_memory_set, (uintptr_t)ptr, size);
  return ptr;
}


void crunch_free(void *ptr)
{
  crunch_set_delete(&runtime_memory_set, (uintptr_t)ptr);
  free(ptr);
}


DYLD_INTERPOSE(crunch_malloc, malloc);
DYLD_INTERPOSE(crunch_free, free);
