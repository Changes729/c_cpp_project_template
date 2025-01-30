#include <algorithm>
#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <queue>

using namespace std;

//! 此题超时

struct TreeNode
{
  int val;
  TreeNode *left;
  TreeNode *right;
  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Codec
{
public:
  // Encodes a tree to a single string.
  string serialize(TreeNode *root)
  {
    string res;
    queue<TreeNode *> record;
    record.push(root);

    while (!record.empty())
    {
      TreeNode *node = record.front();
      if (node != NULL)
      {
        res = res + to_string(node->val);
        record.push(node->left);
        record.push(node->right);
      }
      res = res + ',';
      record.pop();
    }

    return res;
  }

  // Decodes your encoded data to tree.
  TreeNode *deserialize(string data)
  {
    TreeNode *root = NULL;
    queue<TreeNode **> record;

    record.push(&root);
    size_t pos = 0;
    size_t new_pos = 0;
    std::string delimiter = ",";

    while ((new_pos = data.find_first_of(delimiter, pos)) != std::string::npos)
    {
      string token = data.substr(pos, new_pos - pos);

      if (record.empty())
      {
        break;
      }
      TreeNode **node = record.front();
      if (token.length() > 0)
      {
        int val = stoi(token);
        *node = new TreeNode(val);
        record.push(&(*node)->left);
        record.push(&(*node)->right);
      }
      record.pop();

      pos = new_pos + delimiter.length();
    }

    return root;
  }
};

// Your Codec object will be instantiated and called as such:
// Codec* ser = new Codec();
// Codec* deser = new Codec();
// string tree = ser->serialize(root);
// TreeNode* ans = deser->deserialize(tree);
// return ans;