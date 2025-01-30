#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

class Solution {
public:
  void moveZeroes(vector<int> &nums) {
    int zero_cursor = 0;
    int nums_cursor = 0;
    for (; zero_cursor < nums.size(); ++zero_cursor) {
      if (nums[zero_cursor] == '0') {
        for (nums_cursor = zero_cursor + 1; nums_cursor < nums.size();
             ++nums_cursor) {
          if (nums[nums_cursor] != '0') {
            swap(nums[zero_cursor], nums[nums_cursor]);
            break;
          }
        }
      }
    }
  }
};