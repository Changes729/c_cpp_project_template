#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;
class Solution {
public:
  bool makesquare(vector<int> &matchsticks) {
    auto sum = accumulate(matchsticks.begin(), matchsticks.end(), 0);
    if (0 != (sum & 0x03)) {
      return false;
    }

    bool res = true;
    auto avg = sum >> 2;
    sort(matchsticks.begin(), matchsticks.end(), greater<int>());
    vector<int> edge(4, 0);

    return dfs(0, matchsticks, edge, avg);
  }

  bool dfs(int index, vector<int> &matchsticks, vector<int> &edges, int len) {
    if (index == matchsticks.size()) {
      return true;
    }

    for (int i = 0; i < edges.size(); ++i) {
      edges[i] += matchsticks[index];
      if (edges[i] <= len && dfs(index + 1, matchsticks, edges, len)) {
        return true;
      }
      edges[i] -= matchsticks[index];
    }
    return false;
  }
};

void check_vector(vector<int> vec) {
  cout << Solution().makesquare(vec) << endl;
}

int main(void) {
  //   check_vector({{1, 2}});
  check_vector({1, 1, 2, 2, 2});
  check_vector({1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2});
  check_vector({5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3});
  return 0;
}