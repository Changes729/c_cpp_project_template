#include <iostream>

template<typename T>
class deferClass
{
  const T f;

public:
  deferClass(const T &f_)
      : f(f_)
  {}
  ~deferClass() { f(); }
};

#define CONCATENATE_IMPLEMENT(x, y) x##y
#define CONCATENATE(x, y)           CONCATENATE_IMPLEMENT(x, y)
#define defer                       deferClass CONCATENATE(defer, __LINE__) =

int main()
{
  deferClass end = []() { std::cout << "defer code(0)" << std::endl; };
  defer[]() { std::cout << "defer code(1)" << std::endl; };
  defer[]() { std::cout << "defer code(2)" << std::endl; };

  std::cout << "main code" << std::endl;
  return 0;
}