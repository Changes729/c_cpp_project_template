#include <iostream>
#include <vector>

using namespace std;

class SolutionA {
public:
  vector<int> findRightInterval(vector<vector<int>> &intervals) {
    vector<int> right_arr(intervals.size(), -1);
    for (int i = 0; i < intervals.size(); i++) {
      for (int j = 0; j < intervals.size(); j++) {
        if (i == j) {
          continue;
        } else if (intervals[j][0] >= intervals[i][1]) {
          if (right_arr[i] == -1 ||
              intervals[right_arr[i]][0] > intervals[j][0]) {
            right_arr[i] = j;
          }
        }
      }
    }

    return right_arr;
  }
};

class Solution {
public:
  vector<int> findRightInterval(vector<vector<int>> &intervals) {
    vector<pair<int, int>> records;
    for (int i = 0; i < intervals.size(); i++) {
      records.push_back(make_pair(i, intervals[i][0]));
    }

    qsort(&records[0], records.size(), sizeof(pair<int, int>),
          [](const void *a, const void *b) {
            const pair<int, int> *vec_a = (const pair<int, int> *)a;
            const pair<int, int> *vec_b = (const pair<int, int> *)b;
            return (*vec_a).second - (*vec_b).second;
          });

    vector<int> res(intervals.size(), -1);
    for (int i = 0; i < intervals.size(); ++i) {
      res[i] = find_index(records, intervals[i][1]);
    }
    return res;
  }

  int find_index(vector<pair<int, int>> &records, int num) {
    int res = -1;
    int left = 0;
    int right = records.size() - 1;

    while (left <= right) {
      int mid = (left + right) / 2;
      if (records[mid].second >= num) {
        if (res == -1) {
          res = records[mid].first;
        } else {
          res = min(res, records[mid].first);
        }
        right = mid - 1;
      } else {
        left = mid + 1;
      }
    }

    return res;
  }
};

void check_vector(vector<vector<int>> vec) {
  vector<int> res = Solution().findRightInterval(vec);
  for (auto &i : res) {
    cout << i << " ";
  }
    cout << endl;
}

int main(void) {
//   check_vector({{1, 2}});
  check_vector({{3, 4}, {2, 3}, {1, 2}});
  check_vector({{1, 4}, {2, 3}, {3, 4}});
  return 0;
}