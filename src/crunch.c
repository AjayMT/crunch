
#ifndef _CRUNCH_C_
#define _CRUNCH_C_


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "crunch_set.c"


#define DYLD_INTERPOSE(_replacement,_replacee)                          \
  __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
  __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };


void crunch_init() __attribute__((constructor));
void crunch_finish() __attribute__((destructor));
void crunch_dump_heap();
void *crunch_malloc(size_t);
void crunch_free(void*);


DYLD_INTERPOSE(crunch_malloc, malloc);
DYLD_INTERPOSE(crunch_free, free);


static crunch_set_t malloc_set;


void crunch_init()
{
  crunch_set_init(&malloc_set);
}


void crunch_finish()
{
  fprintf(stderr, "\n=== crunch ===\n");
  fprintf(stderr, "%llu remaining heap allocations\n", malloc_set.count);
  for (uint64_t index = 0; index < malloc_set.capacity; ++index) {
    crunch_block_t block = malloc_set.blocks[index];
    if (block.ptr == 0) continue;
    fprintf(stderr, "  %p : %lu bytes\n", (void *)block.ptr, block.size);
  }

  if (*getenv("CRUNCH_DUMP_HEAP") == 'y')
    crunch_dump_heap();

  crunch_set_destroy(&malloc_set);
}


void crunch_dump_heap()
{
  fprintf(stderr, "\ndumping heap... "); fflush(stderr);

  for (uint64_t index = 0; index < malloc_set.capacity; ++index) {
    crunch_block_t block = malloc_set.blocks[index];
    if (block.ptr == 0) continue;

    char fname[250];
    sprintf(fname, "%s/%p", getenv("CRUNCH_OUT"), (void *)block.ptr);
    FILE *out_file = fopen(fname, "w");

    unsigned char *data = (unsigned char *)block.ptr;
    for (size_t i = 0; i < block.size; ++i)
      fputc(data[i], out_file);

    fclose(out_file);
  }

  fprintf(stderr, "done.\n");
}


void *crunch_malloc(size_t size)
{
  void *ptr = malloc(size);
  crunch_set_put(&malloc_set, (uintptr_t)ptr, size);
  return ptr;
}


void crunch_free(void *ptr)
{
  crunch_set_delete(&malloc_set, (uintptr_t)ptr);
  free(ptr);
}


#endif /* _CRUNCH_C_ */
