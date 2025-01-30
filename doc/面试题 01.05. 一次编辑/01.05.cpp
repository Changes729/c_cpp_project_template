#include <iostream>
#include <string.h>

bool oneEditAway(char *first, char *second)
{
  {
    int length[] = {strlen(first), strlen(second)};
    auto diff = length[1] - length[0];
    if (diff == 1)
    {
      auto tmp = second;
      second = first;
      first = tmp;
    }

    if (abs(diff) >= 2)
    {
      return false;
    }
  }


  for(int i = 0; first[i] != '\0' && second[i] != '\0'; i++)
  {
    if(first[i] != second[i])
    {
      return (strcmp(first + i + 1, second + i) == 0) || (strcmp(first + i + 1, second + i + 1) == 0);
    }
  }

  return true;
}