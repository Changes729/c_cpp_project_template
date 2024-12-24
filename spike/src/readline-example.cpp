#include <stdio.h>
#include <stdlib.h>

#include "readline/readline.h"

int main(int argc, char** argv)
{
  char* line = readline("my-rl-example> ");
  printf("You entered: \"%s\"\n", line);

  free(line);

  return 0;
}