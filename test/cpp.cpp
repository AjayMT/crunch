
#include <iostream>


int main(int argc, char *argv[])
{
  int *n = new int;
  *n = 12;
  delete n;

  return 0;
}
