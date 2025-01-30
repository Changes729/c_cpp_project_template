#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  vector<int> fallingSquares(vector<vector<int>> &positions) {
    vector<int> res(positions.size());
    int highest = 0;
    for (int i = 0; i < positions.size(); i++) {
      auto item = positions[i];
      int left_1 = item[0], right_1 = left_1 + item[1] - 1, high = item[1];
      res[i] = high;
      for (int j = 0; j < i; j++) {
        auto cmp_item = positions[j];
        int left_2 = cmp_item[0], right_2 = left_2 + cmp_item[1] - 1;
        if (left_1 <= right_2 && right_1 >= left_2) {
          res[i] = max(res[i], res[j] + high);
        }
      }
    }

    for (int i = 1; i < positions.size(); i++) {
      res[i] = max(res[i], res[i - 1]);
    }

    return res;
  }
};
