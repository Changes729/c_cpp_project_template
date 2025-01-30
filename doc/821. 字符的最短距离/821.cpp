#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class Solution {
public:
  vector<int> shortestToChar(string s, char c) {
    vector<int> result;
    result.resize(s.length());

    vector<int> char_index_arr;
    for (int i = 0; i < s.length(); ++i) {
      if (s[i] == c) {
        char_index_arr.emplace_back(i);
      }
    }

    set_index(result, 0, char_index_arr[0], false);
    for (int i = 1; i < char_index_arr.size(); ++i) {
      int half = (char_index_arr[i] - char_index_arr[i - 1]) >> 1;
      int remain = (char_index_arr[i] - char_index_arr[i - 1]) & 0x01;
      int start = char_index_arr[i - 1];
      int end = char_index_arr[i];
      set_index(result, start, start + half + remain, true);
      set_index(result, end - half, end, false);
    }
    set_index(result, char_index_arr[char_index_arr.size() - 1], s.length() - 1, true);

    return result;
  }

  void set_index(vector<int> &array, int start, int end, bool from_front) {
    if (from_front) {
      for (int i = 0; start + i <= end; ++i) {
        array[start + i] = i;
      }
    } else {
      for (int i = 0; start <= end - i; ++i) {
        array[end - i] = i;
      }
    }
  }
};