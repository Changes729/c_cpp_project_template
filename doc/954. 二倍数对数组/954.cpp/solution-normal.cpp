#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;
class Solution {
public:
  bool canReorderDoubled(vector<int> &arr) {
    multiset<int> singal_num;

    for (auto num : arr) {
      singal_num.insert(num);
    }

    multiset<float> record;
    auto itr_less_0 = singal_num.lower_bound(0);
    for (auto itr = singal_num.begin(); itr != itr_less_0; itr++) {
      int num = *itr;
      auto find_itr = record.find(num);
      if (find_itr != record.end()) {
        record.erase(find_itr);
      } else {
        record.insert(num / 2.0);
      }
    }

    auto itr_up_0 = singal_num.upper_bound(0);
    for (auto itr = itr_up_0; itr != singal_num.end(); itr++) {
      int num = *itr;
      auto find_itr = record.find(num);
      if (find_itr != record.end()) {
        record.erase(find_itr);
      } else {
        record.insert(num * 2);
      }
    }

    return record.empty();
  }
};