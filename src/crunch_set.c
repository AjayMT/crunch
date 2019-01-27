
#include <stdlib.h>
#include <stdint.h>


static const uint64_t initial_set_capacity = 0xffff;
static const unsigned int set_growth_factor = 2;


typedef struct {
  uintptr_t ptr;
  size_t size;
} crunch_block_t;


typedef struct {
  crunch_block_t *blocks;
  uint64_t count;
  uint64_t capacity;
} crunch_set_t;


void crunch_set_init(crunch_set_t*);
void crunch_set_destroy(crunch_set_t*);
void crunch_set_put(crunch_set_t*, uintptr_t, size_t);
size_t crunch_set_get(crunch_set_t, uintptr_t);
void crunch_set_delete(crunch_set_t*, uintptr_t);
static uint64_t crunch_hash_ptr(uint64_t, uintptr_t);
static crunch_block_t *crunch_set_at(crunch_set_t, uintptr_t);
static void crunch_set_grow(crunch_set_t*);


void crunch_set_init(crunch_set_t *set)
{
  set->blocks = calloc(initial_set_capacity, sizeof(crunch_block_t));
  set->count = 0;
  set->capacity = initial_set_capacity;
}


void crunch_set_destroy(crunch_set_t *set)
{
  set->capacity = 0;
  set->count = 0;
  free(set->blocks);
}


void crunch_set_put(crunch_set_t *set, uintptr_t ptr, size_t size)
{
  if (set == NULL || set->blocks == NULL || set->capacity == 0 || ptr == 0 || size == 0)
    return;

  if (set->count == set->capacity)
    crunch_set_grow(set);

  uint64_t index = crunch_hash_ptr(set->capacity, ptr);
  crunch_block_t *block = (set->blocks) + index;
  for (; block->ptr != 0; ++block);

  block->ptr = ptr;
  block->size = size;
  ++(set->count);
}


size_t crunch_set_get(crunch_set_t set, uintptr_t ptr)
{
  crunch_block_t *block = crunch_set_at(set, ptr);
  if (block == NULL) return 0;
  return block->size;
}


void crunch_set_delete(crunch_set_t *set, uintptr_t ptr)
{
  if (set == NULL) return;

  crunch_block_t *block = crunch_set_at(*set, ptr);
  if (block == NULL) return;

  block->ptr = 0;
  block->size = 0;
  --(set->count);
}


static uint64_t crunch_hash_ptr(uint64_t cap, uintptr_t ptr)
{
  return (uint64_t)(((__uint128_t)ptr * (__uint128_t)cap) >> 64);
  // return ptr % set.capacity;
}


static crunch_block_t *crunch_set_at(crunch_set_t set, uintptr_t ptr)
{
  if (set.blocks == NULL || set.count == 0 || ptr == 0) return NULL;

  uint64_t index = crunch_hash_ptr(set.capacity, ptr);
  crunch_block_t *block = set.blocks + index;

  for (; block->ptr != ptr; ++block)
    if (block - set.blocks >= set.capacity)
      return NULL;

  return block;
}


static void crunch_set_grow(crunch_set_t *set)
{
  uint64_t new_cap = (set->capacity) * set_growth_factor;
  uint64_t old_cap = set->capacity;
  set->capacity = new_cap;
  crunch_block_t *new_blocks = calloc(new_cap, sizeof(crunch_block_t));
  crunch_block_t *old_blocks = set->blocks;
  set->blocks = new_blocks;
  set->count = 0;

  for (uint64_t index = 0; index < old_cap; ++index) {
    crunch_block_t block = old_blocks[index];
    if (block.ptr == 0) continue;
    crunch_set_put(set, block.ptr, block.size);
  }

  free(old_blocks);
}
