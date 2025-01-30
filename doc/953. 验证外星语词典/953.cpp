#include <iostream>
#include <vector>

using namespace std;
class Solution {
private:
  vector<int> weight;

public:
  void weight_init(const string &order) {
    weight.resize(order.length());

    for (int i = 0; i < order.length(); i++) {
      weight[order[i] - 'a'] = i + 'a';
    }
  }

  int translate_ch(char c) {
    int res = 0;
    if (c != 0) {
      res = weight[c - 'a'];
    }
    return res;
  }

  bool isAlienSorted(vector<string> &words, string order) {
    int total_visit = 0;
    vector<int> visit(words.size(), 0);
    weight_init(order);

    bool res = true;
    for (int rank = 0; total_visit < words.size() - 1 && true == res; rank++) {
      for (int i = 1; i < words.size(); i++) {
        if (0 == visit[i - 1]) {
          int diff =
              translate_ch(words[i][rank]) - translate_ch(words[i - 1][rank]);

          if (diff >= 0) {
            if (diff > 0 || words[i - 1][rank] == '\0') {
              visit[i - 1] = true;
              total_visit = total_visit + 1;
            } else if (words[i][rank] == '\0') {
              res = false;
              break;
            }
          } else {
            res = false;
            break;
          }
        }
      }
    }

    return res;
  }
};

int main(void) {
  vector<string> words = {"hello", "leetcode"};
  string order = "hlabcdefgijkmnopqrstuvwxyz";
  cout << Solution().isAlienSorted(words, order) << endl;

  words = {"hl", "h"};
  cout << Solution().isAlienSorted(words, order) << endl;

  words = {"kuvp","q"};
  order = "ngxlkthsjuoqcpavbfdermiywz";
  cout << Solution().isAlienSorted(words, order) << endl;

  return 0;
}