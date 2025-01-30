#include <iostream>

using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
  TreeNode *inorderSuccessor(TreeNode *root, TreeNode *p) {
    TreeNode *res = find_pre_order_next(root, p);
    if (res->val == p->val) {
      res = NULL;
    }
    return res;
  }

  TreeNode *find_pre_order_next(TreeNode *node, const TreeNode *p) {
    TreeNode *res = NULL;
    if (node == NULL)
      return NULL;

    res = find_pre_order_next(node->left, p);
    if (res != NULL) {
      if (res->val == p->val) {
        res = node;
      }
    } else if (node->val == p->val) {
      res = find_first(node->right);
      if (res == NULL) {
        res = node;
      }
    } else {
      res = find_pre_order_next(node->right, p);
    }

    return res;
  }

  TreeNode *find_first(TreeNode *node) {
    TreeNode* res = node;

    if(node != NULL && node->left != NULL)
    {
      res  = find_first(node->left);
    }

    return res;
  }
};