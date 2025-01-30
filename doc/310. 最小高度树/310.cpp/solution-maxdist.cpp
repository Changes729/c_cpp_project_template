#include <algorithm>
#include <cctype>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

/**
 * 本题解的解体基础是最小高度树一定是最大高度树的二分之一。
 * 所以本题的思路从寻找对小高度树，转化为寻找最大高度树。
 *
 * Q1：如何寻找最大高度树？https://leetcode-cn.com/problems/minimum-height-trees/solution/by-ac_oier-7xio/
 * Q2：如何通过最大高度树确定最小高度树根节点？
 */
class Solution {
public:
  vector<int> findMinHeightTrees(int n, vector<vector<int>> &edges) {
    // 构造邻接链表
    vector<vector<int>> adj(n);
    for (auto &edge : edges) {
      adj[edge[0]].emplace_back(edge[1]);
      adj[edge[1]].emplace_back(edge[0]);
    }

    vector<int> parent_path(n);
    int x = bfs(0, parent_path, adj);
    int y = bfs(x, parent_path, adj);

    vector<int> max_path;
    while (y != x) {
      max_path.emplace_back(y);
      y = parent_path[y];
    }
    max_path.emplace_back(x);

    int max_size = max_path.size();
    if (max_size % 2 == 0) {
      return {max_path[max_size / 2 - 1], max_path[max_size / 2]};
    } else {
      return {max_path[max_size / 2]};
    }
  }

  int bfs(int v, vector<int> &parent_path, vector<vector<int>> &adj) {

    int n = adj.size();
    queue<int> record;
    vector<bool> visit(n);
    int last_node;
    record.emplace(v);
    visit[v] = true;

    while (!record.empty()) {
      int curr_v = record.front();
      record.pop();
      last_node = curr_v;
      for (auto &next_v : adj[curr_v]) {
        if (!visit[next_v]) {
          visit[next_v] = true;
          record.emplace(next_v);
          parent_path[next_v] = curr_v;
        }
      }
    }

    return last_node;
  }
};