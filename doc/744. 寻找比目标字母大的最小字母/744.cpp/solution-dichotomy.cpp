#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  char nextGreatestLetter(vector<char> &letters, char target) {
    size_t size = letters.size();
    size_t l = 0, r = size - 1;

    while (l < r) {
      int mid = l + r >> 1;
      if (letters[mid] > target)
        r = mid;
      else
        l = mid + 1;
    }

    return letters[r] > target ? letters[r] : letters[0];
  }
};