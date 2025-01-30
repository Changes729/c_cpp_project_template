#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int numSubarrayProductLessThanK(int *nums, int numsSize, int k) {
  int res = 0;

  int begin_index = 0;
  int end_index = 0;
  int multi = 1;

  while (end_index < numsSize) {
    multi = multi * nums[end_index];
    end_index = end_index + 1;

    while (begin_index < end_index && multi >= k) {
      multi = multi / nums[begin_index];
      begin_index = begin_index + 1;
      res = res + (end_index - begin_index);
    }
  }

  while (begin_index < end_index) {
    multi = multi / nums[begin_index];
    res = res + (end_index - begin_index);
    begin_index = begin_index + 1;
  }

  return res;
}