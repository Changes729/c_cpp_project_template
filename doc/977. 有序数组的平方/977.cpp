#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  vector<int> sortedSquares(vector<int> &nums) {
    vector<int> array;
    array.resize(nums.size());

    int begin = 0, end = nums.size() - 1;
    int index = end;
    int pow_begin = nums[begin] * nums[begin];
    int pow_end = nums[end] * nums[end];
    while (index > 0) {
      int biggest;
      if (pow_begin >= pow_end) {
        biggest = pow_begin;
        begin += 1;
        pow_begin = nums[begin] * nums[begin];
      } else {
        biggest = pow_end;
        end -= 1;
        pow_end = nums[end] * nums[end];
      }
      array[index] = biggest;
      --index;
    }
    array[0] = pow_begin;
    return array;
  }
};