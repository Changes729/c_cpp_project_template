#include <string>

using namespace std;

class Solution {
public:
  bool reachingPoints(int sx, int sy, int tx, int ty) {
    bool res = false;
    do {
      if (tx == sx && ty == sy) {
        res = true;
        break;
      }

      if (tx >= ty) {
        tx -= ty;
      } else {
        ty -= tx;
      }

    } while (tx > 0 && ty > 0);
    return res;
  }
};