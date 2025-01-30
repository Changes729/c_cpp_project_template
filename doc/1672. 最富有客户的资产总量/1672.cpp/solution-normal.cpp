#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  int maximumWealth(vector<vector<int>> &accounts) {
    int max_wealth = 0;
    for (auto &account : accounts) {
      int curr_wealth = 0;
      for (auto &property : account) {
        curr_wealth += property;
      }
      if (max_wealth < curr_wealth) {
        max_wealth = curr_wealth;
      }
    }
    return max_wealth;
  }
};