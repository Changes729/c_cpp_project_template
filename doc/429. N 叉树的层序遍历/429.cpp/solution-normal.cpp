#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;
// Definition for a Node.
class Node {
public:
  int val;
  vector<Node *> children;

  Node() {}

  Node(int _val) { val = _val; }

  Node(int _val, vector<Node *> _children) {
    val = _val;
    children = _children;
  }
};

class Solution {
public:
  vector<vector<int>> levelOrder(Node *root) {
    vector<vector<int>> tree;
    if (root == NULL) {
      return {};
    }

    int curr_max_node = 1;
    queue<Node *> nQueue;
    nQueue.emplace(root);

    vector<int> curr_level;
    while (!nQueue.empty()) {
      Node *curr_node = nQueue.front();

      if (curr_node != NULL) {
        curr_level.push_back(curr_node->val);
        for (auto node : curr_node->children) {
          nQueue.emplace(node);
        }
      }

      nQueue.pop();
      --curr_max_node;

      if (curr_max_node == 0) {
        vector<int> new_level;
        tree.push_back(move(curr_level));
        curr_level = move(new_level);
        curr_max_node = nQueue.size();
      }
    }

    return tree;
  }
};