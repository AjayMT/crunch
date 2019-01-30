
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


void *thread_routine(void *arg)
{
  void *ptr = malloc(17);
  free(ptr);
  return NULL;
}


int main(int argc, char *argv[])
{
  for (unsigned int i = 0; i < 100; ++i) {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_routine, NULL);
  }
  usleep(500);
  return 0;
}
