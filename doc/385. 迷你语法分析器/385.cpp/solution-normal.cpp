#include <algorithm>
#include <cctype>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

// This is the interface that allows for creating nested lists.
// You should not implement it, or speculate about its implementation
class NestedInteger {
public:
  // Constructor initializes an empty nested list.
  NestedInteger();
  // Constructor initializes a single integer.
  NestedInteger(int value);
  // Return true if this NestedInteger holds a single integer, rather than a
  // nested list.
  bool isInteger() const;
  // Return the single integer that this NestedInteger holds, if it holds a
  // single integer The result is undefined if this NestedInteger holds a nested
  // list
  int getInteger() const;
  // Set this NestedInteger to hold a single integer.
  void setInteger(int value);
  // Set this NestedInteger to hold a nested list and adds a nested integer to
  // it.
  void add(const NestedInteger &ni);
  // Return the nested list that this NestedInteger holds, if it holds a nested
  // list The result is undefined if this NestedInteger holds a single integer
  const vector<NestedInteger> &getList() const;
};

class Solution {
public:
  NestedInteger deserialize(string s) {
    NestedInteger nest_int;
    _deserialize(nest_int, s, 0);
    return nest_int.getList()[0];
  }

  size_t _deserialize(NestedInteger &nest_int, string &s, size_t begin) {
    size_t cursor_edge = begin;
    size_t cursor_end = s.length();
    size_t cursor_num = cursor_edge;
    const char *str = s.c_str();

    if (cursor_edge >= cursor_end) {
      return cursor_end;
    }

    while (cursor_edge < cursor_end) {
      cursor_edge = s.find_first_of("[]", cursor_edge);

      while (cursor_num < cursor_edge) {
        nest_int.add(atoi(str + cursor_num));
        cursor_num = s.find_first_of(",", cursor_num);
        if (cursor_num != string::npos) {
          cursor_num = cursor_num + 1;
        }
      }

      if (cursor_edge < cursor_end) {
        if (s[cursor_edge] == '[') {
          NestedInteger new_nest;
          cursor_edge = _deserialize(new_nest, s, cursor_edge + 1);
          nest_int.add(new_nest);
          cursor_num = s.find_first_of(",", cursor_edge);
          if (cursor_num != string::npos) {
            cursor_num = cursor_num + 1;
          }
        } else if (s[cursor_edge] == ']') {
          return cursor_edge + 1;
        }
      }
    }

    return cursor_end;
  }
};