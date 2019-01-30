
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
  int *m = malloc(12);
  int *n = malloc(13);
  int *z = malloc(25);
  printf("test-leak %p %p %p\n", m, n, z);

  return 0;
}
