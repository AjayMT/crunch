
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
  int *n = malloc(sizeof(int));
  *n = SIGABRT;
  raise(*n);
  return 0;
}
