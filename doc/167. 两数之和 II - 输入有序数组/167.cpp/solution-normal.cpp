#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  vector<int> twoSum(vector<int> &numbers, int target) {
    int index_x = 0, index_y = numbers.size() - 1;
    while (index_x < index_y) {
      auto sum = numbers[index_x] + numbers[index_y];
      if (sum < target) {
        ++index_x;
      } else if (sum > target) {
        index_x = 0;
        --index_y;
      } else {
        break;
      }
    }

    return {index_x + 1, index_y + 1};
  }
};