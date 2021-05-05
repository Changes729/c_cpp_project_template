/** See a brief introduction (right-hand button) */
#include "dbus_epoll.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "io-flush.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void on_dbus_watch(void *user_data, fd_desc_t pkg)
{
  DBusWatch *  watch = user_data;
  unsigned int flags = 0;

  if(pkg.flag & IO_NOTICE_READ) flags |= DBUS_WATCH_READABLE;
  if(pkg.flag & IO_NOTICE_WRITE) flags |= DBUS_WATCH_WRITABLE;
  if(pkg.flag & IO_NOTICE_HUP) flags |= DBUS_WATCH_HANGUP;
  if(pkg.flag & IO_NOTICE_ERR) flags |= DBUS_WATCH_ERROR;

  while(!dbus_watch_handle(watch, flags)) {
    printf("dbus_watch_handle needs more memory\n");
    break;
  }
}

dbus_bool_t _add_watch(DBusWatch *watch, void *data)
{
  unsigned  flags = dbus_watch_get_flags(watch);
  fd_desc_t desc  = {.fd = dbus_watch_get_unix_fd(watch), .flag = 0};

  if(flags & DBUS_WATCH_READABLE) desc.flag |= IO_NOTICE_READ;
  if(flags & DBUS_WATCH_WRITABLE) desc.flag |= IO_NOTICE_WRITE;

  io_notice_file(desc, on_dbus_watch, watch);

  return 1;
}

void _remove_watch(DBusWatch *watch, void *data)
{
  io_ignore_file(dbus_watch_get_unix_fd(watch));
}

void _toggled_watch(DBusWatch *watch, void *data)
{
  if(dbus_watch_get_data(watch)) {
    _add_watch(watch, data);
  } else {
    _remove_watch(watch, data);
  }
}
