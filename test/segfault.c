
#include <stdlib.h>


int main(int argc, char *argv[])
{
  int *n = malloc(sizeof(int));
  *n = 65;
  int *p = (int *)0xdeadbeef;
  *p = 12;
  free(n);
  return 0;
}
