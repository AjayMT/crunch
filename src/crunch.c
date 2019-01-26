
#include <stdlib.h>
#include <stdio.h>

#include "crunch.h"
#include "khash.h"


static size_t malloc_count = 0;
static size_t free_count = 0;


void crunch_init()
{}


void crunch_finish()
{
  printf("%lu calls to malloc\n", malloc_count);
  printf("%lu calls to free\n", free_count);
}


void *crunch_malloc(size_t size)
{
  ++malloc_count;
  void *ptr = malloc(size);
  return ptr;
}


void crunch_free(void *ptr)
{
  ++free_count;
  free(ptr);
}


DYLD_INTERPOSE(crunch_malloc, malloc);
DYLD_INTERPOSE(crunch_free, free);
