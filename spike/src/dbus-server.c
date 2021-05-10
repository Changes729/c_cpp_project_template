#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

#include "dbus_define.h"
#include "dbus_helper.h"
#include "dbus_initial.h"
#include "dbus_object.h"
#include "dbus_task.h"
#include "io-flush.h"
#include "timer-task.h"

static void dbus_main_loop(DBusConnection *connection);
static void sigint_handler(int param);
void        dbus_dispatch_status(DBusConnection *   connection,
                                 DBusDispatchStatus new_status,
                                 void *             data);

static int main_loop_running = 1;

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
  DBusConnection *connection;

  if(!dbus_init(DBUS_BUS_SESSION,
                TEST_DBUS_BUS_NAME,
                DBUS_NAME_FLAG_REPLACE_EXISTING,
                &connection))
  {
    goto __failed;
  }

  signal(SIGINT, sigint_handler);
  register_dbus_object_path(connection);

  timer_task_new(1000, print_time, NULL);

  dbus_main_loop(connection);

  unregister_dbus_object_path(connection);

__failed:
  return 0;
}

static void dbus_main_loop(DBusConnection *connection)
{
  while(main_loop_running) {
    io_flush_select(timer_next_alarm() >> 1);

    timer_run();
  }
}

static void sigint_handler(int param)
{
  main_loop_running = 0;
}
