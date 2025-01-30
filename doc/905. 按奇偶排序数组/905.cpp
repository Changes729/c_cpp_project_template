#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;
class Solution {
public:
    vector<int> sortArrayByParity(vector<int>& nums) {
        vector<int> res = nums;
        int index_front = 0, index_rear = res.size() - 1;
        while(index_front < index_rear)
        {
            swap(res[index_front], res[index_rear]);
            while((res[index_front] & 0x01) == 0x00)
            {
                index_front = index_front + 1;
                if(index_rear < index_front)
                {
                    break;
                }
            }
            
            while((res[index_rear] & 0x01) == 0x01)
            {
                index_rear = index_rear - 1;
                if(index_rear < index_front)
                {
                    break;
                }
            }
        }

        return res;
    }
};
