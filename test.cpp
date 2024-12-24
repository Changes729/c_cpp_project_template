#include <stddef.h>

#include <functional>
#include <iostream>

#define ERROR_PART 0

template<typename T>
class _Var
{
public:
  using F = std::function<bool(T)>;
  bool __HOOK_DEF_TRUE(T) { return true; };

public:
  explicit _Var(T right, F hook = nullptr)
      : _var{right}
      , _hook_set{
            hook == nullptr
                ? std::bind(&_Var::__HOOK_DEF_TRUE, this, std::placeholders::_1)
                : hook}
  {}

  inline                operator const T() { return _var; }
  inline const _Var<T> &operator=(const _Var<T> &right)
  {
    return this->operator=(right._s);
  }

  inline const _Var<T> &operator=(const T &right)
  {
    if(_hook_set(right)) {
      _var = right;
    }
    return *this;
  }

private:
  T _var;
  F _hook_set;
};

class IA
{
public:
  IA()
      : enable{false, std::bind(&IA::hook_enable, this, std::placeholders::_1)}
  {}

  void func() { std::cout << (enable ? "Enable" : "disable") << std::endl; }

  _Var<bool> enable;

private:
  inline bool hook_enable(bool b) { return _hook_enable(b); }

  virtual bool _hook_enable(bool b) { return true; }

private:
  bool _enable_hook(bool) { return true; };
};

class A_test: public IA
{
protected:
  virtual bool _hook_enable(bool b) override
  {
    if(b == enable) {
      std::cout << "enable is same" << std::endl;
    }
    return true;
  };

public:
  void update_val(bool val) { enable = val; }
};

int main(void)
{
  if(0) {
    _Var<int> _int{1};
    std::cout << _int << std::endl;
    _int = 2;
    std::cout << _int << std::endl;
#if ERROR_PART
    _int = 1.6;
    std::cout << _int << std::endl;
#endif

    _Var<const int> _const_int{1};
    std::cout << _const_int << std::endl;
#if ERROR_PART
    _const_int = 3;
    std::cout << _const_int << std::endl;
#endif

    _Var<double> _double{0.2};
    std::cout << _double << std::endl;
    _double = 1.2;
    std::cout << _double << std::endl;
    _double = 1;
    std::cout << _double << std::endl;

    _Var<uint8_t> _u8{1};
    std::cout << _u8 << std::endl;
    _u8 = 0xFF;
    std::cout << _u8 << std::endl;
#if ERROR_PART
    _u8 = 0xFFFF;
    std::cout << _u8 << std::endl;
#endif

    _Var<bool> _bool{false};
    std::cout << _bool << std::endl;
    _bool = 5;
    std::cout << _bool << std::endl;
  }

  A_test a_test;

  a_test.func();
  a_test.update_val(true);
  a_test.func();
  a_test.update_val(true);

  std::cout << std::is_void_v<void> << std::endl;
  std::cout << std::is_void_v<int> << std::endl;

  return 0;
}