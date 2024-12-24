#include <assert.h>
#include <stdio.h>

#include <iostream>

template<class C>
class Instance
{
public:
  static C& instance() { return *_instance; }
  template<typename... __arg_pkg>
  static C* create(__arg_pkg... args)
  {
    assert(_instance == nullptr);
    _instance = new C(args...);
    assert(_instance != nullptr);
    return _instance;
  }
  static void destroy()
  {
    delete _instance;
    _instance = nullptr;
  }

private:
  static C* _instance;
};

template<class C>
C* Instance<C>::_instance = nullptr;

class B: public Instance<B>
{
private:
  friend Instance<B>;
  B(int i)
      : _i{i}
  {}

public:
  void print() { std::cout << "hello world: " << _i << std::endl; }

private:
  int _i;
};

int main(void)
{
  B::create(1);
  auto b = B::instance();

  b.print();

  return 0;
}
