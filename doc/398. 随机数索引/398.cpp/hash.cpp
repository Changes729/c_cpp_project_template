#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

using namespace std;

class Solution {
public:
  Solution(vector<int> &nums) : _hash{} {
    for (int i = 0; i < nums.size(); i++) {
      int num = nums[i];
      auto itr = _hash.find(num);
      if (itr == _hash.end()) {
        auto res = _hash.emplace(make_pair(num, vector<int>{}));
        if (true == res.second) {
          itr = res.first;
        }
      }

      if (itr != _hash.end()) {
        itr->second.emplace_back(i);
      }
    }
  }

  int pick(int target) {
    int index = -1;
    auto itr = _hash.find(target);
    if (itr != _hash.end()) {
      int rand_num = rand() % itr->second.size();
      index = itr->second[rand_num];
    }

    return index;
  }

private:
  map<int, vector<int>> _hash;
};

/**
 * Your Solution object will be instantiated and called as such:
 * Solution* obj = new Solution(nums);
 * int param_1 = obj->pick(target);
 */