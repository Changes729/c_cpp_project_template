#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "io-flush.h"

static bool _quit = false;

static void stdin_callback(void* user_data, fd_desc_t pkg)
{
  char buffer[256] = {};
  int  result      = read(pkg.fd, &buffer, sizeof(buffer));

  if(result > 0) {
    printf("%s\n", buffer);

    if(strncmp("quit", buffer, strlen("quit")) == 0) {
      _quit = true;

      io_ignore_file(0);
    }
  }
}

int main(void)
{
  io_notice_file((fd_desc_t){0, IO_NOTICE_READ}, stdin_callback, NULL);

  while(!_quit) {
    io_flush_select();
  }

  return 0;
}