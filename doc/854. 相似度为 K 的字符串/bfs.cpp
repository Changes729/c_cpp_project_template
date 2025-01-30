#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>

using namespace std;

class Solution
{
public:
  int kSimilarity(string s1, string s2)
  {
    int                      swap_times = 0;
    queue<pair<string, int>> processing_que;
    unordered_set<string>    records;

    processing_que.emplace(s1, swap_times);
    records.emplace(s1);

    while(!processing_que.empty()) {
      auto [processing_str, times] = processing_que.front();
      processing_que.pop();

      if(processing_str == s2) {
        swap_times = times;
        break;
      }

      for(int i = 0;; i++) {
        if(processing_str[i] != s2[i]) {
          auto require = i;
          while(++require < processing_str.length()) {
            if(s2[require] != processing_str[require] &&
               s2[require] == processing_str[i]) {
              swap(processing_str[require], processing_str[i]);
              if(records.count(processing_str) == 0) {
                records.emplace(processing_str);
                processing_que.emplace(processing_str, times + 1);
              }
              swap(processing_str[require], processing_str[i]);
            }
          }
          break;
        }
      }
    }

    return swap_times;
  }
};

int main(void)
{
  cout << Solution().kSimilarity("aabbccddee", "dcacbedbae");

  return 0;
}