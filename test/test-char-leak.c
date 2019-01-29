
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
  char *c = malloc(strlen("quuxbAAz"));
  strcpy(c, "quuxbAAz");
  return 0;
}
