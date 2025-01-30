#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  int trailingZeroes(int n) {
    size_t count_0 = 0;
    size_t count_5 = 0;
    for (int i = 5; i <= n; i += 5) {
      int tmp_n = i;

      while (tmp_n % 10 == 0) {
        count_0++;
        tmp_n = tmp_n / 10;
      }

      while (tmp_n % 5 == 0) {
        count_5++;
        tmp_n = tmp_n / 5;
      }
    }

    return count_0 + count_5;
  }
};
