
#include <stdlib.h>


int main(int argc, char *argv[])
{
  char *buf = malloc(10);
  buf[0xffffff] = 'a';
  free(buf);
  return 0;
}
