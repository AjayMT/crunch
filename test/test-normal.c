
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
  int *n = malloc(sizeof(int));
  *n = 12;
  free(n);
  printf("test-normal exit\n");
  return 0;
}
