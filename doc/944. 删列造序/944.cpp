#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include <string.h>

using namespace std;

class Solution
{
public:
  int minDeletionSize(char **strs, int strsSize)
  {
    const int STR_LEN = strlen(strs[0]);
    int remove[STR_LEN];
    memset(remove, 0, sizeof(remove));

    for (int i = 1; i < strsSize; i++)
    {
      for (int j = 0; j < STR_LEN; j++)
      {
        remove[j] = remove[j] | (strs[i][j] < strs[i - 1][j]);
      }
    }

    int res = 0;
    for (int j = 0; j < STR_LEN; j++)
    {
      res = res + remove[j];
    }

    return res;
  }
};