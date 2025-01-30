#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;


class Solution {
public:
  vector<int> missingRolls(vector<int> &rolls, int mean, int n) {
    vector<int> arrays;

    long sum = mean * (rolls.size() + n);
    for (auto num : rolls) {
      sum = sum - num;
    }

    auto n_means = sum / n;
    auto n_remainder = sum - n_means * n;

    if (n_means > 6 || n_means <= 0) {
      return arrays;
    }

    for (int i = 0; i < n; ++i) {
      if (n_remainder > 0) {
        if (n_means + 1 > 6) {
          break; // has no number bigger than 6.
        }

        arrays.push_back(n_means + 1);
        n_remainder = n_remainder - 1;
      } else {
        arrays.push_back(n_means);
      }
    }

    return arrays;
  }
};