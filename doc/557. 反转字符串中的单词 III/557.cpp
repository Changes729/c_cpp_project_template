#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
class Solution {
public:
  string reverseWords(string s) {
    std::vector<char> str(s.begin(), s.end());
    int start = 0;
    for(int i = 0; i < str.size(); i++)
    {
      if(str[i] == ' ')
      {
        reverseString(str, start, i);
        start = i + 1;
      }
    }
    reverseString(str, start, str.size());

    return string(str.begin(), str.end());
  }

  void reverseString(vector<char> &s, int start, int end) {
    size_t size = end - start;
    for (int i = 0; i < size / 2; ++i) {
      swap(s[start + i], s[start + size - 1 - i]);
    }
  }
};