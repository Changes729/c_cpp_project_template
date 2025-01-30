#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class Solution {
public:
  int projectionArea(vector<vector<int>> &grid) {
    int res = 0;

    vector<int> line_max;
    line_max.resize(grid.size());
    for (auto &column : grid) {
      int line_index = 0;
      int colume_max = 0;
      for (auto &node : column) {
        line_max[line_index] = max(line_max[line_index], node);
        colume_max = max(colume_max, node);
        if (node != 0) {
          res = res + 1;
        }
        line_index = line_index + 1;
      }
      res = res + colume_max;
    }

    for (auto &line : line_max) {
      res = res + line;
    }

    return res;
  }
};