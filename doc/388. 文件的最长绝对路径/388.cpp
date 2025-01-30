#include <algorithm>
#include <cctype>
#include <iostream>
#include <stack>
#include <vector>

using namespace std;

struct tree_t {
  vector<tree_t> node;
  size_t length;
  string str;

  tree_t() : node{}, length{0}, str{""} {}
};

class Solution {
public:
  int lengthLongestPath(string input) {
    size_t input_size = input.length();
    int pos = 0;
    int res = 0;
    stack<int> st;

    while (pos < input_size) {
      int depth = 0;
      while (pos < input_size && input[pos] == '\t') {
        pos++;
        depth++;
      }

      int len = 0;
      bool is_file = false;
      while (pos < input_size && input[pos] != '\n') {
        if (input[pos] == '.') {
          is_file = true;
        }
        pos++;
        len++;
      }
      pos++; // skip for '\n'

      while (st.size() > depth) {
        st.pop();
      }

      if (!st.empty()) {
        len = len + st.top() + 1;
      }

      if (is_file) {
        res = max(res, len);
      } else {
        st.emplace(len);
      }
    }

    return res;
  }
};
