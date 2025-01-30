#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include <numeric>

using namespace std;

class Solution
{
public:
    vector<int> diStringMatch(string s)
    {
        vector<int> nums(s.length() + 1);
        iota(nums.begin(), nums.end(), 0);

        int index_begin = 0;
        int index_end = s.length() - 1;
        while (index_begin <= index_end)
        {
            if (s[index_begin] == 'D' && s[index_end] == 'D')
            {
                swap(nums[index_begin], nums[index_end + 1]);
                index_begin = index_begin + 1;
                index_end = index_end - 1;
            }

            while (index_begin <= index_end && s[index_begin] != 'D')
            {
                index_begin = index_begin + 1;
            }

            while (index_begin <= index_end && s[index_end] != 'D')
            {
                index_end = index_end - 1;
            }
        }

        return nums;
    }
};