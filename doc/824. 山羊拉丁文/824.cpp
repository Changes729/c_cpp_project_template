#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;
class Solution {
public:
  enum status_t {
    S_STR_SPACE,
    S_STR_PHASING,
  } _status;

  string toGoatLatin(string sentence) {
    size_t pos = 0;
    string ret;

    size_t index = 1;
    char first_ch = '\0';
    _status = S_STR_SPACE;
    for (auto ch : sentence) {
      switch (_status) {
      case S_STR_SPACE:
        if (ch == ' ') {
          ret += ' ';
        } else {
          if (!is_vowels(ch)) {
            first_ch = ch;
          } else {
            ret += ch;
          }
          _status = S_STR_PHASING;
        }
        break;
      case S_STR_PHASING:
        if (ch != ' ') {
          ret += ch;
        } else {
          if (first_ch != '\0') {
            ret += first_ch;
            first_ch = '\0';
          }
          ret += "ma";

          for (int i = index; i > 0; --i) {
            ret += 'a';
          }

          ret += ' ';
          index += 1;
          _status = S_STR_SPACE;
        }
        break;

      default:
        break;
      }
    }

    if (first_ch != '\0') {
      ret += first_ch;
      first_ch = '\0';
    }
    ret += "ma";

    for (int i = index; i > 0; --i) {
      ret += 'a';
    }

    return ret;
  }

  bool is_vowels(char c) {
    switch (c) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
    case 'A':
    case 'E':
    case 'I':
    case 'O':
    case 'U':
      return true;
      break;

    default:
      return false;
      break;
    }
  }
};