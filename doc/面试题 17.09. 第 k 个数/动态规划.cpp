#include <iostream>
#include <set>
#include <vector>

using namespace std;
class Solution
{
public:
  int getKthMagicNumber(int k)
  {
    const int         prime_arr[]  = {3, 5, 7};
    vector<long long> records      = {1};
    size_t            index_arr[3] = {0, 0, 0};
    vector<long long> val_arr      = {3, 5, 7};

    for(int i = 0; i < k - 1; ++i) {
      long long val = *min_element(val_arr.begin(), val_arr.end());

      records.emplace_back(val);
      for(int j = 0; j < val_arr.size(); ++j) {
        if(val_arr[j] == val) {
          index_arr[j]++;
          val_arr[j] = records[index_arr[j]] * prime_arr[j];
        }
      }
    }

    return records[k - 1];
  }
};