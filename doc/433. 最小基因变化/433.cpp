#include <algorithm>
#include <iostream>
#include <math.h>
#include <vector>
#include <queue>

using namespace std;

class Solution
{
private:
    struct Node
    {
        string data;
        vector<Node> childs;
    };

public:
    int minMutation(string start, string end, vector<string> &bank)
    {
        Node root{start, {}};
        vector<int> visit(bank.size(), 0);
        queue<pair<Node &, int>> q;

        q.push({root, 1});
        while (!q.empty())
        {
            auto &pair = q.front();
            Node &node = pair.first;
            int visit_index = pair.second;
            int child_num = insert_child(node, visit, bank, visit_index);
            for (int i = 0; i < child_num; i++)
            {
                q.push({node.childs[i], visit_index + 1});
            }
            q.pop();
        }

        int res = -1;
        for (int i = 0; i < bank.size(); i++)
        {
            if (end == bank[i])
            {
                if (visit[i] != 0)
                {
                    res = visit[i];
                }
            }
        }

        return res;
    }

    int insert_child(Node &root, vector<int> &visit, const vector<string> &bank, int visit_id)
    {
        for (int i = 0; i < bank.size(); i++)
        {
            if (visit[i] != 0)
            {
                continue;
            }

            if (calculate_distance(bank[i], root.data) == 1)
            {
                visit[i] = visit_id;
                root.childs.emplace_back(Node{bank[i], {}});
            }
        }

        return root.childs.size();
    }

    int calculate_distance(const string &A, const string &B)
    {
        int distance = 0;
        for (int i = 0; i < A.length(); i++)
        {
            distance = distance + (A[i] != B[i]);
        }
        return distance;
    }
};