#include <algorithm>
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

class Solution {
public:
  int largestPalindrome(int n) {
    long long a = pow(10, n) - 1;
    long long b = a;
    int EDGE = pow(10, n - 1);
    long long largest = 0;
    while (a >= EDGE) {
      while (b >= EDGE) {
        long long result = a * b;
        if (is_palindrome(result)) {
          // printf("%d %d", a, b);
          if (result > largest) {
            largest = result;
            EDGE = b;
          }
        }

        if(result < largest)
        {
          break;
        }

        --b;
      }
      b = a;
      --a;
    }

    return largest % 1337;
  }

  bool is_palindrome(long long value) {
    char array[20];
    size_t length = snprintf(array, sizeof(array), "%ld", value);
    int a = 0, b = length - 1;
    bool result = true;
    // printf("size: %d", length);
    while (a < b) {
      if (array[a] != array[b]) {
        result = false;
        break;
      }

      ++a;
      --b;
    }

    return result;
  }
};