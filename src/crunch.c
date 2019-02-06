
#ifndef _CRUNCH_C_
#define _CRUNCH_C_


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "crunch_set.c"


void crunch_init() __attribute__((constructor));
void crunch_finish() __attribute__((destructor));
void crunch_dump_heap();
void crunch_dump_fatal(char*);
void crunch_signal_handler(int, siginfo_t*, void*);
void crunch_invalid_free_handler(uintptr_t);
void *crunch_malloc(size_t);
void crunch_free(void*);


const size_t max_msg_size = 250;
const char msg_terminator = '\t';
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

  struct sigaction action_sigsegv;
  memset(&action_sigsegv, 0, sizeof(struct sigaction));
  action_sigsegv.sa_flags = SA_SIGINFO;
  action_sigsegv.sa_sigaction = crunch_signal_handler;
  sigaction(SIGSEGV, &action_sigsegv, NULL);

  struct sigaction action_sigabrt;
  memset(&action_sigabrt, 0, sizeof(struct sigaction));
  action_sigabrt.sa_flags = SA_SIGINFO;
  action_sigabrt.sa_sigaction = crunch_signal_handler;
  sigaction(SIGABRT, &action_sigabrt, NULL);
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

  if (*getenv("CRUNCH_REPORT") == 'y')
    crunch_dump_heap();

  crunch_set_destroy(&malloc_set);
  pthread_mutex_destroy(&malloc_mutex);
}


void crunch_dump_heap()
{
  const char *CRUNCH_OUT = "CRUNCH_OUT";
  fprintf(stderr, "\ndumping heap...\n");

  char stats_fname[max_msg_size];
  char stats_data[max_msg_size];
  sprintf(stats_fname, "%s/stats", getenv(CRUNCH_OUT));
  sprintf(
    stats_data,
    "%llu\n%llu\n%llu\n%llu\n%c",
    malloc_count, free_count, max_usage, current_usage, msg_terminator
    );

  FILE *stats_file = fopen(stats_fname, "w");
  for (
    unsigned int i = 0;
    i < max_msg_size && stats_data[i] != msg_terminator;
    ++i
    ) fputc(stats_data[i], stats_file);

  fclose(stats_file);

  for (uint64_t index = 0; index < malloc_set.capacity; ++index) {
    crunch_block_t block = malloc_set.blocks[index];
    if (block.ptr == 0) continue;

    char fname[max_msg_size];
    sprintf(fname, "%s/heap/%p", getenv(CRUNCH_OUT), (void *)block.ptr);
    FILE *out_file = fopen(fname, "w");

    unsigned char *data = (unsigned char *)block.ptr;
    for (size_t i = 0; i < block.size; ++i)
      fputc(data[i], out_file);

    fclose(out_file);
  }

  fprintf(stderr, "heap contents written to %s/heap\n", getenv(CRUNCH_OUT));
}


void crunch_dump_fatal(char *buf)
{
  fprintf(stderr, "\nwriting crash info...\n");

  char fname[max_msg_size];
  sprintf(fname, "%s/fatal", getenv("CRUNCH_OUT"));

  FILE *fatal_file = fopen(fname, "w");
  for (
    unsigned int i = 0;
    i < max_msg_size && buf[i] != msg_terminator;
    ++i
    ) fputc(buf[i], fatal_file);

  fclose(fatal_file);

  fprintf(stderr, "crash info written to %s\n", fname);
}


void crunch_signal_handler(int signum, siginfo_t *info, void *context)
{
  char sig_msg[max_msg_size];
  switch (signum) {
  case SIGSEGV:
    strcpy(sig_msg, "SIGSEGV illegal access"); break;
  case SIGABRT:
    strcpy(sig_msg, "SIGABRT abort");
  }

  fprintf(stderr, "\ncrunch: %s: address %p\n", sig_msg, info->si_addr);

  if (*getenv("CRUNCH_REPORT") == 'y') {
    char msg[max_msg_size];
    sprintf(msg, "%s: address %p\n%c", sig_msg, info->si_addr, msg_terminator);
    crunch_dump_fatal(msg);
  }

  exit(1);
}


void crunch_invalid_free_handler(uintptr_t ptr)
{
  fprintf(stderr, "\ncrunch: pointer being freed was not allocated: %p\n", (void *)ptr);

  if (*getenv("CRUNCH_REPORT") == 'y') {
    char msg[max_msg_size];
    sprintf(msg, "pointer being freed was not allocated: %p\n%c", (void *)ptr, msg_terminator);
    crunch_dump_fatal(msg);
  }

  exit(1);
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
  } else {
    // TODO
    // crunch_invalid_free_handler((uintptr_t)ptr);
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
