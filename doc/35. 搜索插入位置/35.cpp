#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  int searchInsert(vector<int> &nums, int target) {
    int handle_l = 0, handle_r = nums.size() - 1;
    int target_index = -1;
    while (handle_l < handle_r) {
      int handle_mid = (handle_l + handle_r) / 2;
      if (nums[handle_mid] < target) {
        handle_l = handle_mid + 1;
      } else if (target < nums[handle_mid]) {
        handle_r = handle_mid - 1;
      } else {
        target_index = handle_mid;
        break;
      }
    }

    if(handle_l >= handle_r)
    {
      int handle_mid = (handle_l + handle_r) / 2;
      if (nums[handle_mid] < target) {
        target_index = handle_mid + 1;
      } else {
        target_index = handle_mid;
      }
    }

    return target_index;
  }
};