#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  int calPoints(vector<string> &ops) {
    vector<int> nums;
    nums.reserve(ops.size());

    size_t index = 0;
    for (auto value : ops) {
      if (value.compare("C") == 0) {
        index -= 2;
      } else if (value.compare("D") == 0) {
        nums[index] = nums[index - 1] * 2;
      } else if (value.compare("+") == 0) {
        nums[index] = nums[index - 1] + nums[index - 2];
      } else {
        nums[index] = atoi(value.c_str());
      }

      index++;
    }

    long sum = 0;
    for (int i = 0; i < index; i++) {
      sum += nums[i];
    }
    return sum;
  }
};