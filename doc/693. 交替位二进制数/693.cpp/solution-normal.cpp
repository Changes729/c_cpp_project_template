#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

#define XNOR(a, b) !(((a)&0x01) ^ ((b)&0x01))

class Solution {
public:
  bool hasAlternatingBits(int n) {
    bool alternating = true;
    for (; n > 0; n >>= 1) {
      if (XNOR(n, n >> 1)) {
        alternating = false;
        break;
      }
    }

    return alternating;
  }
};