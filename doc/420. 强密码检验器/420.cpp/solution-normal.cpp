#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  int strongPasswordChecker(string password) {
    int res = 0;

    bool has_digit = false;
    bool has_upper = false;
    bool has_lower = false;
    int count_repeat = false;
    int repeat_times = 0;
    char record = 0;
    for (auto ch : password) {
      if (isdigit(ch)) {
        has_digit = true;
      } else if (isupper(ch)) {
        has_upper = true;
      } else if (islower(ch)) {
        has_lower = true;
      }

      if (record != ch) {
        if (repeat_times >= 3) {
          count_repeat += (repeat_times / 3);
        }
        record = ch;
        repeat_times = 1;
      } else if (record == ch) {
        repeat_times += 1;
      }
    }

    if (repeat_times >= 3) {
      count_repeat += (repeat_times / 3);
    }

    int lack = 3 - (has_digit + has_lower + has_upper);
    int supp_count = 0;
    if (password.length() < 6) {
      supp_count = 6 - password.length();
      return max(supp_count, lack);
    } else if (password.length() > 20) {
      supp_count = password.length() - 20;
      count_repeat = remove_char(password, supp_count);
    }

    return supp_count + max(count_repeat, lack);
  }

  int remove_char(string password, int count) {

    vector<int> sub_str_count;

    int count_repeat = false;
    int repeat_times = 0;
    char record = 0;
    for (auto ch : password) {
      if (record != ch) {
        if (repeat_times >= 3) {
          sub_str_count.push_back(repeat_times);
        }
        record = ch;
        repeat_times = 1;
      } else if (record == ch) {
        repeat_times += 1;
      }
    }
    if (repeat_times >= 3) {
      sub_str_count.push_back(repeat_times);
    }

    size_t size = sub_str_count.size();
    int i = 0;
    while (count > 0 && size > 0) {
      for (auto &num : sub_str_count) {
        if (num % 3 == i) {
          if (num == 3 + i) {
            num = 0;
            size--;
          } else {
            num -= (1 + i);
          }
          count -= (i + 1);
          if (count <= 0) {
            num = num - count;
            break;
          }
          if (size == 0) {
            break;
          }
        } else if (num == 0) {
          continue;
        }
      }
      i++;
      i = i % 3;
      if (i + 1 > count) {
        break;
      }
    }

    if (size) {
      int sum = 0;
      for (auto num : sub_str_count) {
        sum += num / 3;
      }

      return sum;
    } else {
      return 0;
    }
  }
};