
#ifndef _CRUNCH_C_
#define _CRUNCH_C_


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "crunch_set.c"


void crunch_init() __attribute__((constructor));
void crunch_finish() __attribute__((destructor));
void crunch_dump_heap();
void *crunch_malloc(size_t);
void crunch_free(void*);


uint64_t malloc_count = 0;
uint64_t free_count = 0;
uint64_t max_usage = 0;
uint64_t current_usage = 0;
crunch_set_t malloc_set;
pthread_mutex_t malloc_mutex;


void crunch_init()
{
  crunch_set_init(&malloc_set);
  pthread_mutex_init(&malloc_mutex, NULL);
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
  pthread_mutex_destroy(&malloc_mutex);
}


void crunch_dump_heap()
{
  const char *CRUNCH_OUT = "CRUNCH_OUT";
  const size_t fnamesize = 250;
  fprintf(stderr, "\ndumping heap...\n");

  char stats_fname[fnamesize];
  char stats_data[fnamesize];
  sprintf(stats_fname, "%s/stats", getenv(CRUNCH_OUT));
  sprintf(
    stats_data,
    "%llu\n%llu\n%llu\n%llu\nN",
    malloc_count, free_count, max_usage, current_usage
    );

  FILE *stats_file = fopen(stats_fname, "w");
  for (unsigned int i = 0; i < fnamesize; ++i) {
    if (stats_data[i] == 'N') break;
    fputc(stats_data[i], stats_file);
  }
  fclose(stats_file);

  for (uint64_t index = 0; index < malloc_set.capacity; ++index) {
    crunch_block_t block = malloc_set.blocks[index];
    if (block.ptr == 0) continue;

    char fname[fnamesize];
    sprintf(fname, "%s/%p", getenv(CRUNCH_OUT), (void *)block.ptr);
    FILE *out_file = fopen(fname, "w");

    unsigned char *data = (unsigned char *)block.ptr;
    for (size_t i = 0; i < block.size; ++i)
      fputc(data[i], out_file);

    fclose(out_file);
  }

  fprintf(stderr, "heap contents written to %s\n", getenv(CRUNCH_OUT));
}


void *crunch_malloc(size_t size)
{
  pthread_mutex_lock(&malloc_mutex);
  void *ptr = malloc(size);

  if (crunch_set_put(&malloc_set, (uintptr_t)ptr, size)) {
    ++malloc_count;
    current_usage += size;
    if (current_usage > max_usage) max_usage = current_usage;
  }

  pthread_mutex_unlock(&malloc_mutex);
  return ptr;
}


void crunch_free(void *ptr)
{
  pthread_mutex_lock(&malloc_mutex);

  size_t size = crunch_set_get(malloc_set, (uintptr_t)ptr);
  if (size) {
    ++free_count;
    current_usage -= size;
  }

  crunch_set_delete(&malloc_set, (uintptr_t)ptr);
  free(ptr);

  pthread_mutex_unlock(&malloc_mutex);
}


#define DYLD_INTERPOSE(_replacement,_replacee)                          \
  __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
  __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };


DYLD_INTERPOSE(crunch_malloc, malloc);
DYLD_INTERPOSE(crunch_free, free);


#endif /* _CRUNCH_C_ */
