#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class Solution {
public:
  int smallestRangeI(vector<int> &nums, int k) {
    int min_num = nums[0], max_num = nums[0];

    for (auto &num : nums) {
      min_num = min(min_num, num);
      max_num = max(max_num, num);
    }

    return max(abs(max_num - min_num) - 2 * k, 0);
  }
};