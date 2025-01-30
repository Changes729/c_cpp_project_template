#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

static const int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
class Solution
{
  vector<vector<int>> heights;

public:
  void dfs(int row, int col, vector<vector<bool>> &ocean)
  {
    int m = ocean.size();
    int n = ocean[0].size();
    if (ocean[row][col])
    {
      return;
    }

    ocean[row][col] = true;
    for (int i = 0; i < 4; i++)
    {
      int newRow = row + dirs[i][0], newCol = col + dirs[i][1];
      if (newRow >= 0 && newRow < m && newCol >= 0 && newCol < n && heights[newRow][newCol] >= heights[row][col])
      {
        dfs(newRow, newCol, ocean);
      }
    }
  }

  vector<vector<int>> pacificAtlantic(vector<vector<int>> &heights)
  {
    this->heights = heights;
    long row = heights.size();
    long col = heights[0].size();

    vector<vector<bool>> pacific(row, vector<bool>(col, false));
    vector<vector<bool>> atlantic(row, vector<bool>(col, false));

    for (int i = 0; i < row; i++)
    {
      dfs(i, 0, pacific);
    }
    for (int j = 1; j < col; j++)
    {
      dfs(0, j, pacific);
    }
    for (int i = 0; i < row; i++)
    {
      dfs(i, col - 1, atlantic);
    }
    for (int j = 0; j < col - 1; j++)
    {
      dfs(row - 1, j, atlantic);
    }

    vector<vector<int>> result;
    for (int i = 0; i < row; i++)
    {
      for (int j = 0; j < col; j++)
      {
        if (pacific[i][j] && atlantic[i][j])
        {
          vector<int> cell;
          cell.emplace_back(i);
          cell.emplace_back(j);
          result.emplace_back(cell);
        }
      }
    }

    return result;
  }
};