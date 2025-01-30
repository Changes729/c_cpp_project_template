#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class Solution {
public:
  vector<int> numberOfLines(vector<int> &widths, string s) {
    int lines_count = 1;
    int width_count = 0;
    for (auto &ch : s) {
      width_count += widths[ch - 'a'];
      if (width_count > 100) {
        ++lines_count;
        width_count = widths[ch - 'a'];
      }
    }

    return {lines_count, width_count};
  }
};