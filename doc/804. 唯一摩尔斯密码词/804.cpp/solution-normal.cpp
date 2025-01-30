#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

const char *MOS_CODE_MAP[]{
    ".-",   "-...", "-.-.", "-..",  ".",   "..-.", "--.",  "....", "..",
    ".---", "-.-",  ".-..", "--",   "-.",  "---",  ".--.", "--.-", ".-.",
    "...",  "-",    "..-",  "...-", ".--", "-..-", "-.--", "--.."};

class Solution {

public:
  int uniqueMorseRepresentations(vector<string> &words) {
    set<string> unique_str;

    for (auto &str : words) {
      string new_str;
      for (auto ch : str) {
        new_str += MOS_CODE_MAP[ch - 'a'];
      }
      unique_str.emplace(new_str);
    }

    return unique_str.size();
  }
};