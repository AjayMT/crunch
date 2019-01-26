
#include <stdio.h>
#include <stdlib.h>

#include "crunch.h"


void *crunch_malloc(size_t size)
{
  fprintf(stderr, "allocating %lu bytes\n", size);
  return malloc(size);
}


void crunch_free(void *ptr)
{
  fprintf(stderr, "free");
  free(ptr);
}


DYLD_INTERPOSE(crunch_malloc, malloc);
DYLD_INTERPOSE(crunch_free, free);
