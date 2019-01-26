
#include <stdlib.h>
#include <stdint.h>


static const uint64_t initial_set_size = 64;
static const float set_growth_factor = 1.5;


typedef struct {
  uint64_t ptr;
  size_t size;
} _crunch_block_t;


typedef struct {
  _crunch_block_t *blocks;
  uint64_t size;
  uint64_t capacity;
} crunch_set_t;


void crunch_set_init(crunch_set_t*);
void crunch_set_destroy(crunch_set_t*);
void crunch_set_put(crunch_set_t*, uint64_t, size_t);
size_t crunch_set_get(crunch_set_t*, uint64_t);
static _crunch_block_t *crunch_set_at(crunch_set_t*, uint64_t);
static void crunch_set_grow(crunch_set_t*);


void crunch_set_init(crunch_set_t *set)
{}


void crunch_set_destroy(crunch_set_t *set)
{}


void crunch_set_put(crunch_set_t *set, uint64_t ptr, size_t size)
{}


size_t crunch_set_get(crunch_set_t *set, uint64_t ptr)
{}


static _crunch_block_t *crunch_set_at(crunch_set_t *set, uint64_t ptr)
{}


static void crunch_set_grow(crunch_set_t *set)
{}
