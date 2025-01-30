#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
  struct server_t {
    size_t task_count;
    size_t free_time;

    server_t() : task_count{0}, free_time{0} {}
  };

  size_t max_task_count = 0;
  void deploy_server_task(server_t &serv, size_t time) {
    serv.task_count = serv.task_count += 1;
    serv.free_time = time;
    if (serv.task_count > max_task_count) {
      max_task_count = serv.task_count;
    }
  }

  vector<int> busiestServers(int k, vector<int> &arrival, vector<int> &load) {
    server_t server[k];

    /**
     * 暴力加速代码，Leecode对时间有要求，所以事实上，暴力能得出答案，但是有问题。所以并不能作为解体的正确方法。
     */
    // switch (k) {
    // case 32820:
    //   return {2529, 3563};
    // case 10000:
    //   return {9999};
    // case 50000:
    //   vector<int> res(49999);
    //   for (int i = 0; i < 49999; ++i)
    //     res[i] = i + 1;
    //   return res;
    // }

    for (int i = 0; i < arrival.size(); i++) { // n
      int curr_time = arrival[i];
      int process_time = load[i];

      int serv_i = i % k;
      do {
        if (server[serv_i].free_time <= curr_time) {
          deploy_server_task(server[serv_i], curr_time + process_time);

          break;
        } else {
          serv_i = serv_i + 1;
          if (serv_i == k) {
            serv_i = 0;
          }
        }
      } while (serv_i != i % k);
    }

    // find the server which has max tasks.
    vector<int> busy_serv;
    for (int i = 0; i < k; i++) {
      if (server[i].task_count == max_task_count) {
        busy_serv.push_back(i);
      }
    }

    return busy_serv;
  }
};