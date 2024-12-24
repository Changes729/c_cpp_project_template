#include <stddef.h>

#include <iostream>
/**
 * @brief from: https://github.com/zyuchuan/unique_cpp/blob/master/ch01_cpp_metaprogramming.md
 */
/** 循环 */
template<size_t N>
struct factorial
{
  static constexpr size_t value = N * factorial<N - 1>{}.value;
};
// 递归必须要有结束条件，一般用一个特化的模板来作为结束条件
template<>
struct factorial<1>
{
  static constexpr size_t value = 1;
};

/** 分支 */
/* SFINAE - Substitution Failure Is Not An Error */
template<class T>
typename T::multiplication_result multiply(T t1, T t2)
{
  return t1 * t2;
}

long multiply(int i, int j) { return i * j; }

#include <iostream>
#include <type_traits>

using namespace std;

// #1
template<class T>
typename enable_if<is_pointer<T>::value, void>::type do_something(T)
{
  cout << "calling do_something(T*), return nothing\n";
}

// #2
template<class T>
typename enable_if<!is_pointer<T>::value, T>::type do_something(T t)
{
  cout << "calling do_something(T), return " << t << endl;
  return t;
}

int main(void)
{
  /** 常量声明 */
  typedef integral_constant<int, 2> two_type;
  typedef integral_constant<int, 6> six_type;

  typedef integral_constant<bool, true>  true_type;
  typedef integral_constant<bool, false> false_type;

  static_assert(two_type::value * 3 == six_type::value, "2*3 != 6");

  /** 循环 */                                     /** 阶乘为例 */
  std::cout << factorial<5>::value << std::endl;  // 20

  /** 分支 */

  int i = 3;
  do_something(i);
  do_something(&i);

  return 0;
}