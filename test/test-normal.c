
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
  int *n = malloc(sizeof(int));
  *n = 12;
  printf("test-normal %p\n", n);
  free(n);
  return 0;
}
