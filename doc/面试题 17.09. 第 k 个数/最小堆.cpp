#include <iostream>
#include <set>
#include <vector>

using namespace std;

class Solution
{
public:
  int getKthMagicNumber(int k)
  {
    vector<long long> heap;
    set<long long>    records;
    heap.emplace_back(1);

    for(int i = 0; i < k; ++i) {
      const int prime_arr[] = {3, 5, 7};
      long long       curr        = heap[i];

      for(long long prime: prime_arr) {
        long long val = curr * prime;
        if(records.find(val) == records.end()) {
          records.emplace(val);
          heap.emplace_back(val);
        }
      }

      sort(heap.begin(), heap.end());
    }

    return heap[k - 1];
  }
};