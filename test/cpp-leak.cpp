
#include <iostream>


struct node
{
  int n;
  double v;
  char z;
};


int main(int argc, char *argv[])
{
  std::cout << "test-cpp-leak begin" << std::endl;
  node *ptr = new node;

  std::cout << "test-cpp-leak end " << ptr << std::endl;
  return 0;
}
