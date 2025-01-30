#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  int countNumbersWithUniqueDigits(int n) {
    int num = 1;
    while (n > 0) {
      num += combination(n);
      --n;
    }

    return num;
  }

  int combination(int n) {
    int num = 9;
    for (int i = n - 1, conb = 9; i > 0; --i, --conb) {
      num *= conb;
    }
    return num;
  }
};