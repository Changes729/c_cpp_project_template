#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

using namespace std;

// The API isBadVersion is defined for you.
// bool isBadVersion(int version);

class Solution {
public:
  int firstBadVersion(int n) {
    int earliest_version = n;
    unsigned handle_l = 1, handle_r = n;
    while (handle_l <= handle_r) {
      int handle_m = static_cast<long long>(handle_l + handle_r) >> 1;
      if (isBadVersion(handle_m)) {
        earliest_version = handle_m;
        handle_r = handle_m - 1;
      }
      else
      {
        handle_l = handle_m + 1;
      }
    }
    return earliest_version;
  }
};