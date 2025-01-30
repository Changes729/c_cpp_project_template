#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

class RecentCounter
{
private:
    vector<long> time_record;
    int index_start;
    int index_end;

public:
    RecentCounter() : time_record{}, index_start{0}, index_end{0}
    {
        time_record.clear();
    }

    int ping(int t)
    {
        time_record.emplace_back(t);
        index_end = index_end + 1;

        while (t - time_record[index_start] > 3000)
        {
            index_start = index_start + 1;
        }
        return index_end - index_start;
    }
};
