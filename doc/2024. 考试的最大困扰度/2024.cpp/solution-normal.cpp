#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

#define NEED_REMOVE 0x01
#define REMOVE_CMP 0x01

class Solution {
public:
  const int T = 1;
  const int F = 0;
  int K;
  int record[2] = {0, 0};
  size_t count_max = 0;

  int del_world(int T_or_F) {
    int ret = 0;
    if (record[T_or_F] == K + 1) {
      ret = REMOVE_CMP;
    }

    record[T_or_F] = record[T_or_F] - 1;
    return ret;
  }

  int new_world(int T_or_F) {
    int ret = 0;
    record[T_or_F] = record[T_or_F] + 1;

    if (record[F] > K && record[T] > K) {
      ret = NEED_REMOVE;
      size_t count = record[F] + record[T] - 1;
      if (count > count_max) {
        count_max = count;
      }
    }

    return ret;
  }

  int maxConsecutiveAnswers(string answerKey, int k) {
    size_t str_len = answerKey.length();
    K = k;

    for (auto begin = answerKey.begin(), record_begin = begin;
         begin != answerKey.end(); begin++) {
      int ret = 0;

      switch (*begin) {
      case 'T':
        ret = new_world(T);
        break;

      case 'F':
        ret = new_world(F);
        break;

      default:
        continue;
        break;
      }

      if (ret == NEED_REMOVE) {
        int ret = 0;
        while (ret != REMOVE_CMP) {

          switch (*record_begin) {
          case 'T':
            ret = del_world(T);
            break;

          case 'F':
            ret = del_world(F);
            break;

          default:
            continue;
            break;
          }

          record_begin++;
        }
      }
    }

    size_t count = record[F] + record[T];
    if (count > count_max) {
      count_max = count;
    }

    return count_max;
  }
};