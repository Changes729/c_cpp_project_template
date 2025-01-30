#include <algorithm>
#include <iostream>
#include <set>
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

    int storyage[k];
    int start_i = -1;
    for (int record_i = 0; record_i < k; record_i++) {
      storyage[record_i] = -1;
    }

    for (int i = 0; i < arrival.size() + k; i++) { // n
      int serv_i = i % k;

      if (i < arrival.size()) {
        storyage[serv_i] = i;
      }

      int stor_i = serv_i + 1;
      do {
        if (stor_i == k) {
          stor_i = 0;
        }

        if (storyage[stor_i] == -1) {
          // skip
        } else if (server[serv_i].free_time <= arrival[storyage[stor_i]]) {
          deploy_server_task(server[serv_i], arrival[storyage[stor_i]] +
                                                 load[storyage[stor_i]]);
          storyage[stor_i] = -1;
        }
        stor_i++;
      } while (stor_i != serv_i + 1);
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