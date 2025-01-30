#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  int search(vector<int> &nums, int target) {
    int target_index = -1;
    int handle_left = 0, handle_right = nums.size() - 1;
    while (handle_left <= handle_right) {
      int handle_middle = (handle_left + handle_right) >> 1;
      if (nums[handle_middle] < target) {
        handle_left = handle_middle + 1;
      } else if (target < nums[handle_middle]) {
        handle_right = handle_middle - 1;
      } else {
        target_index = handle_middle;
        break;
      }
    }

    return target_index;
  }
};