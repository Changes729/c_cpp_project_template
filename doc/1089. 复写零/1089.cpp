#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

using namespace std;
class Solution {
public:
  void duplicateZeros(vector<int> &arr) {
    vector<int> cpy = arr;
    size_t zero_count = 0;
    for (int i = 0; i < cpy.size() - zero_count; ++i) {
      arr[i + zero_count] = cpy[i];
      if (cpy[i] == 0 && i < cpy.size() - zero_count - 1) {
        zero_count += 1;
        arr[i + zero_count] = cpy[i];
      }
    }
  }
};

void check_vector(vector<int> vec, int k) {
  Solution().duplicateZeros(vec);
  for (auto i : vec) {
    cout << i << " ";
  }
  cout << endl;
}

int main(void) {
  check_vector({1, 0, 2, 3, 0, 4, 5, 0}, 2);
  check_vector({0, 0, 0, 0, 0, 0, 0}, 1);
  check_vector({8, 4, 5, 0, 0, 0, 0, 7}, 0);

  return 0;
}