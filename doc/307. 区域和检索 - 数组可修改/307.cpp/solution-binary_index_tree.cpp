#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

using namespace std;

class NumArray {
private:
  size_t _size;
  int *_bin_index_tree;
  vector<int> _nums;

protected:
  int _lowbit(int x) {
    // x != 0
    return x & -x;
  }

  void _add(int index, int val) {
    while (index <= _size) {
      _bin_index_tree[index] += val;
      index += _lowbit(index);
    }
  }

  int _query(int index) {
    int sum = 0;
    while (index > 0) {
      sum += _bin_index_tree[index];
      index -= _lowbit(index);
    }
    return sum;
  }

public:
  NumArray(vector<int> &nums) {
    _nums = nums;
    _size = nums.size();
    _bin_index_tree = new int[nums.size() + 1]{0};

    for (int i = 0; i < _size; ++i) {
      _add(i + 1, nums[i]);
    }
  }

  ~NumArray() { delete[] _bin_index_tree; }

  void update(int index, int val) {
    if (index < _size && index >= 0) {
      _add(index + 1, val - _nums[index]);
      _nums[index] = val;
    }
  }

  int sumRange(int left, int right) {
    return _query(right + 1) - _query(left);
  }
};

/**
 * Your NumArray object will be instantiated and called as such:
 * NumArray* obj = new NumArray(nums);
 * obj->update(index,val);
 * int param_2 = obj->sumRange(left,right);
 */