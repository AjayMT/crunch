
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
  printf("test-leak begin\n");
  int *n = malloc(13);
  *n = 12;
  /* int *j = malloc(20); */
  /* int *k = malloc(45); */
  /* int *l = malloc(72); */
  printf("test-leak end\n");
  return 0;
}
