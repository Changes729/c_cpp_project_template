#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;
// Definition for a QuadTree node.
class Node {
public:
  bool val;
  bool isLeaf;
  Node *topLeft;
  Node *topRight;
  Node *bottomLeft;
  Node *bottomRight;

  Node() {
    val = false;
    isLeaf = false;
    topLeft = NULL;
    topRight = NULL;
    bottomLeft = NULL;
    bottomRight = NULL;
  }

  Node(bool _val, bool _isLeaf) {
    val = _val;
    isLeaf = _isLeaf;
    topLeft = NULL;
    topRight = NULL;
    bottomLeft = NULL;
    bottomRight = NULL;
  }

  Node(bool _val, bool _isLeaf, Node *_topLeft, Node *_topRight,
       Node *_bottomLeft, Node *_bottomRight) {
    val = _val;
    isLeaf = _isLeaf;
    topLeft = _topLeft;
    topRight = _topRight;
    bottomLeft = _bottomLeft;
    bottomRight = _bottomRight;
  }
};

class Solution {
public:
  Node *construct(vector<vector<int>> &grid) {
    Node *quadtree = new Node;
    size_t x = 0, y = 0, w = grid[0].size(), h = grid.size();

    return mk_quadtree(quadtree, grid, x, y, w, h);
  }

  Node *mk_quadtree(Node *root, vector<vector<int>> &grid, size_t x, size_t y,
                    size_t w, size_t h) {
    if (w == 1 && h == 1) {
      root->isLeaf = true;
      root->val = grid[y][x];
    } else {
      Node *left_top = new Node;
      Node *right_top = new Node;
      Node *left_bottom = new Node;
      Node *right_bottom = new Node;

      auto new_w = w / 2;
      auto new_h = h / 2;
      root->topLeft = mk_quadtree(left_top, grid, x, y, new_w, new_h);
      root->topRight = mk_quadtree(right_top, grid, x + new_w, y, new_w, new_h);
      root->bottomLeft = mk_quadtree(left_bottom, grid, x, y + new_h, new_w, new_h);
      root->bottomRight = mk_quadtree(right_bottom, grid, x + new_w, y + new_h, new_w, new_h);

      if (left_top->isLeaf && right_top->isLeaf && left_bottom->isLeaf &&
          right_bottom->isLeaf &&
          (left_top->val == right_top->val &&
           left_top->val == left_bottom->val &&
           left_top->val == right_bottom->val)) {
        root->isLeaf = true;
        root->val = left_top->val;
        delete root->topLeft;
        delete root->topRight;
        delete root->bottomLeft;
        delete root->bottomRight;
        root->topLeft = NULL;
        root->topRight = NULL;
        root->bottomLeft = NULL;
        root->bottomRight = NULL;
      } else {
        root->isLeaf = false;
        root->val = 1;
      }
    }

    return root;
  }
};