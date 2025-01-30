#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  bool can_divid(int num, char cv) {
    bool res = false;
    switch (cv) {
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      res = num % (cv - '0') == 0;
      break;
    case '1':
      res = true;
      break;

    case '0':
    default:
      break;
    }

    return res;
  }

  bool is_self_divid_num(int num) {
    bool res = true;
    char numstr[5] = {0};

    sprintf(numstr, "%d", num);
    for (int i = 0; numstr[i] != '\0'; i++) {
      if (!can_divid(num, numstr[i])) {
        res = false;
        break;
      }
    }

    return res;
  }

  vector<int> selfDividingNumbers(int left, int right) {
    vector<int> res;

    while (left <= right) {
      if (is_self_divid_num(left)) {
        res.push_back(left);
      }

      left++;
    }

    return res;
  }
};