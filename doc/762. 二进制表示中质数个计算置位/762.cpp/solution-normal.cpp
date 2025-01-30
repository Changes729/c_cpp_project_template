#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class Solution {
public:
  int countPrimeSetBits(int left, int right) {
    int sum = 0;

    while (left <= right) {
      if (1 << __builtin_popcount(left) & 0b10100010100010101100) {
        ++sum;
      }
      ++left;
    }

    return sum;
  }
};