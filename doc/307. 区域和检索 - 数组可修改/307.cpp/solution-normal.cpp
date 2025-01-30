#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

using namespace std;

class NumArray {
public:
  NumArray(vector<int> &nums) {
    _size = nums.size();
    _array = new int[nums.size()];
    for (int i = 0; i < _size; ++i) {
      _array[i] = nums[i];
    }
  }

  ~NumArray() { delete[] _array; }

  void update(int index, int val) {
    if (index < _size && index >= 0) {
      _array[index] = val;
    }
  }

  int sumRange(int left, int right) {
    int sum = 0;
    if (right >= _size) {
      right = _size;
    }

    for (; left <= right; left++) {
      sum += _array[left];
    }

    return sum;
  }

private:
  size_t _size;
  int *_array;
};

/**
 * Your NumArray object will be instantiated and called as such:
 * NumArray* obj = new NumArray(nums);
 * obj->update(index,val);
 * int param_2 = obj->sumRange(left,right);
 */