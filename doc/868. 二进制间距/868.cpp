#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class Solution {
public:
  int binaryGap(int n) {
    int num = n;

    int curr_index = 0;
    int last_index = -1;
    int biggest_gap = 0;
    while (num != 0) {
      bool bit = num & 0x01;
      if (bit == true) {
        if (last_index != -1) {
          biggest_gap = max(biggest_gap, curr_index - last_index);
        }

        last_index = curr_index;
      }

      ++curr_index;
      num = num >> 1;
    }

    return biggest_gap;
  }
};