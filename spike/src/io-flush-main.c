#include <stdio.h>
#include <unistd.h>

#include "io-flush.h"

static void stdin_callback(void* user_data, fd_desc_t pkg)
{
  char buffer[256] = {};
  read(pkg.fd, &buffer, sizeof(buffer));

  printf("%s\n", buffer);

  io_ignore_file(pkg.fd);
}

int main(void)
{
  io_notice_file((fd_desc_t){0, IO_NOTICE_READ}, stdin_callback, NULL);

  while(1) {
    io_flush_select();
  }

  return 0;
}