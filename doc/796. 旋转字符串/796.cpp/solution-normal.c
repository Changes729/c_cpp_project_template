#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool rotateString(char *s, char *goal) {
  if (strcmp(s, goal) == 0) {
    return true;
  }

  bool result = false;
  size_t length = strlen(s);

  if (length != strlen(goal)) {
    return false;
  }

  const char *s_handel = s + 1;
  const char *g_handel = goal + length - 1;
  while (*s_handel != '\0') {
    if (s[0] == g_handel[0] && goal[0] == s_handel[0]) {
      size_t front_size = s_handel - s;
      result = (strncmp(s, g_handel, front_size) == 0 &&
                strncmp(goal, s_handel, length - front_size) == 0);
      if (result) {
        break;
      }
    }
    ++s_handel;
    --g_handel;
  }

  return result;
}