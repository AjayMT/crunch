
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
  printf("test-leak begin\n");
  int *n = malloc(13);
  *n = 12;
  printf("test-leak end %p\n", n);
  return 0;
}
