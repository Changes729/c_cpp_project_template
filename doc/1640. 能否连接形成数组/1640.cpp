
class Solution
{
public:
  bool canFormArray(vector<int>& arr, vector<vector<int>>& pieces)
  {
    int i = 0;
    bool find = true;
    vector<bool> visit(pieces.size(), false);
    while(i < arr.size() && find) {
      find = false;
      for(int j = 0; j < pieces.size(); ++j) {
        if(visit[j]) continue;

        auto& piece = pieces[j];
        if(equal(piece.begin(), piece.end(), arr.begin() + i))
        {
          visit[j] = true;
          i += piece.size();
          find = true;
          break;
        }
      }
    }

    return i >= arr.size();
  }
};