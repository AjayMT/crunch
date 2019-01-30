
#include <memory>


int main(int argc, char *argv[])
{
  std::shared_ptr<int> n = std::make_shared<int>(12);
  return 0;
}
