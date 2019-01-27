
#include <stdlib.h>


int main(int argc, char *argv[])
{
  int *n = malloc(sizeof(int));
  *n = 12;
  free(n);
  return 0;
}
