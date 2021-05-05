#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>

#include "dbus_define.h"
#include "dbus_epoll.h"
#include "dbus_helper.h"
#include "dbus_object.h"
#include "io-flush.h"

static void dbus_main_loop(DBusConnection *dbus_address);
static void sigint_handler(int param);

static DBusConnection *dbus_address;

int main(int agrc, char *argv[])
{
  signal(SIGINT, sigint_handler);

  DBusError err;

  dbus_error_init(&err);

  // dbus address
  dbus_address = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if(dbus_error_is_set(&err)) {
    fprintf(stderr, "Connection Error(%s) \n", err.message);
    dbus_error_free(&err);
    goto __failed;
  }

  // bus name
  dbus_bus_request_name(dbus_address,
                        TEST_DBUS_BUS_NAME,
                        DBUS_NAME_FLAG_REPLACE_EXISTING,
                        &err);
  if(dbus_error_is_set(&err)) {
    fprintf(stderr, "Requece name error(%s) \n", err.message);
    dbus_error_free(&err);
    goto __failed;
  }

  if(!dbus_connection_set_watch_functions(dbus_address,
                                          _add_watch,
                                          _remove_watch,
                                          NULL,
                                          NULL,
                                          NULL))
  {
    fprintf(stderr, "dbus_connection_set_watch_functions failed\n");
    goto __failed;
  }

  register_dbus_object_path(dbus_address);

  dbus_main_loop(dbus_address);

  unregister_dbus_object_path(dbus_address);

__failed:
  return 0;
}

static void dbus_main_loop(DBusConnection *dbus_address)
{
  while(TRUE) {
    io_flush_select();

    while(dbus_connection_dispatch(dbus_address) == DBUS_DISPATCH_DATA_REMAINS) {
    }
  }
}

static void sigint_handler(int param)
{
  unregister_dbus_object_path(dbus_address);

  exit(0);
}