
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "crunch.h"
#include "crunch_set.c"


static crunch_set_t runtime_memory_set;


void crunch_init()
{
  crunch_set_init(&runtime_memory_set);
}


void crunch_finish()
{
  fprintf(stderr, "\n=== crunch ===\n");
  fprintf(stderr, "%llu remaining heap allocations:\n", runtime_memory_set.count);
  for (uint64_t index = 0; index < runtime_memory_set.capacity; ++index) {
    crunch_block_t block = runtime_memory_set.blocks[index];
    if (block.ptr == 0) continue;
    fprintf(stderr, "  %p : %lu bytes\n", (void *)block.ptr, block.size);
  }

  crunch_set_destroy(&runtime_memory_set);
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
