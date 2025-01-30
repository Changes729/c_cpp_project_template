#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

using namespace std;
class Solution {
public:
  int findPairs(vector<int> &nums, int k) {
    map<int, bool> records;
    int count = 0;

    if (k != 0) {
      for (auto num : nums) {
        auto record = records.find(num);
        if (record == records.end()) {
          records.insert(make_pair(num, true));
          records.insert(make_pair(num - k, false));
          records.insert(make_pair(num + k, false));
        } else if (record->second == false) {
          record->second = true;

          auto pre_record = records.find(num - k);
          if (pre_record == records.end()) {
            records.insert(make_pair(num - k, false));
          } else if (pre_record->second == true) {
            count += 1;
          }

          auto rear_record = records.find(num + k);
          if (rear_record == records.end()) {
            records.insert(make_pair(num + k, false));
          } else if (rear_record->second == true) {
            count += 1;
          }
        }
      }
    } else if (k == 0) {
      for (auto num : nums) {
        auto record = records.find(num);
        if (record == records.end()) {
          records.insert(make_pair(num, false));
        } else if (record->second == false) {
          record->second = true;
          count += 1;
        }
      }
    }

    return count;
  }
};

void check_vector(vector<int> vec, int k) {
  cout << Solution().findPairs(vec, k) << endl;
}

int main(void) {
  check_vector({3, 1, 4, 1, 5}, 2);
  check_vector({1, 2, 3, 4, 5}, 1);
  check_vector({1, 3, 1, 5, 4}, 0);

  return 0;
}