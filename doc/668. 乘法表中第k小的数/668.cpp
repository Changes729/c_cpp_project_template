#include <iostream>
#include <vector>

using namespace std;
class Solution {
public:
  int findKthNumber(int m, int n, int k) {

    int res = 1;
    int left = 1, right = m * n + 1;

    while (right >= left) {
      int mid = (left + right) / 2;
      int count = countLessNumber(m, n, mid);
      if (count >= k) {
        res = mid;
        right = mid - 1;
      } else if (count < k) {
        left = mid + 1;
      }
    }

    return res;
  }

  int countLessNumber(int m, int n, int num) {
    int count = 0;
    for (int im = 1; im <= m; im++) {
      count = count + min(n, num / im);
    }

    return count;
  }
};

int main(void) {
  cout << Solution().findKthNumber(3, 3, 5) << endl; // 3
  cout << Solution().findKthNumber(2, 3, 6) << endl; // 6
  cout << Solution().findKthNumber(45, 12, 471) << endl; // 312
  cout << Solution().findKthNumber(42, 34, 401) << endl; // 126
  return 0;
}