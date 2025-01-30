#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    void reverseString(vector<char>& s) {
      size_t size = s.size();
      for(int i = 0; i < size / 2; ++i)
      {
        swap(s[i], s[size - 1 - i]);
      }
    }
};