#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : val(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right)
      : val(x), left(left), right(right) {}
};

class Solution {
public:
  void pot(TreeNode *node, vector<int> &buff) {
    if (node == NULL) {
      return;
    }

    pot(node->left, buff);
    buff.emplace_back(node->val);
    pot(node->right, buff);
  }

  vector<int> getAllElements(TreeNode *root1, TreeNode *root2) {
    vector<int> ret;

    pot(root1, ret);
    pot(root2, ret);
    sort(ret.begin(), ret.end());

    return ret;
  }
};