int _sumRootToLeaf(struct TreeNode *root, int num) {
    int sum = 0;
    num = (num << 1) + root->val;
    if (root->left == NULL && root->right == NULL) return num;
    if (root->left) sum += _sumRootToLeaf(root->left, num);
    if (root->right) sum += _sumRootToLeaf(root->right, num);
    return sum;
}

int sumRootToLeaf(struct TreeNode *root) {
    return root ? _sumRootToLeaf(root, 0) : 0;
}