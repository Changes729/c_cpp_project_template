#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

#include "dbus_define.h"
#include "dbus_epoll.h"
#include "dbus_helper.h"
#include "dbus_object.h"
#include "io-flush.h"
#include "timer-task.h"

static void dbus_main_loop(DBusConnection *dbus_address);
static void sigint_handler(int param);
void        dbus_dispatch_status(DBusConnection *   connection,
                                 DBusDispatchStatus new_status,
                                 void *             data);

static DBusConnection *dbus_address;

void print_time(void *NO_USE)
{
  time_t     timer   = time(NULL);
  struct tm *tm_info = localtime(&timer);
  size_t size = strftime(NULL, INTMAX_MAX, "%Y-%m-%d %H:%M:%S", tm_info) + 1;
  char   buffer[size];

  strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
  printf("\r%s", buffer);
  fflush(stdout);

  timer_task_new(1000, print_time, NULL);
}

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

  // todo:
  // dbus_connection_set_timeout_functions(dbus_address, )

  dbus_connection_set_dispatch_status_function(dbus_address,
                                               dbus_dispatch_status,
                                               NULL,
                                               NULL);

  register_dbus_object_path(dbus_address);

  timer_task_new(1000, print_time, NULL);

  dbus_main_loop(dbus_address);

  unregister_dbus_object_path(dbus_address);

__failed:
  return 0;
}

static void dbus_main_loop(DBusConnection *dbus_address)
{
  while(TRUE) {
    io_flush_select(timer_next_alarm() >> 1);

    while(dbus_connection_dispatch(dbus_address) == DBUS_DISPATCH_DATA_REMAINS)
      ;
    timer_flush();
  }
}

static void sigint_handler(int param)
{
  unregister_dbus_object_path(dbus_address);

  exit(0);
}

void dbus_dispatch_status(DBusConnection *   connection,
                          DBusDispatchStatus new_status,
                          void *             data)
{
  switch(new_status) {
    case DBUS_DISPATCH_DATA_REMAINS:
      while(dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS)
        ;
      break;
    case DBUS_DISPATCH_COMPLETE:
      break;
    case DBUS_DISPATCH_NEED_MEMORY:
    default:
      break;
  }
}