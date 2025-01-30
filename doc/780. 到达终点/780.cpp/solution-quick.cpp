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
        tx -= max((tx - sx) / ty, 1) * ty;
      } else {
        ty -= max((ty - sy) / tx, 1) * tx;
      }

    } while (tx > 0 && ty > 0);
    return res;
  }
};