/** See a brief introduction (right-hand button) */
#include "dbus_epoll.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>
#include <sys/poll.h>
#include <string.h>

/* Private define ------------------------------------------------------------*/
#define MAX_WATCHES 100

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct pollfd pollfds[MAX_WATCHES];
static DBusWatch *watches[MAX_WATCHES];
int max_i;

/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
dbus_bool_t _add_watch(DBusWatch *watch, void *data) {
  short cond = POLLHUP | POLLERR;
  int fd;
  unsigned int flags;

  fd = dbus_watch_get_unix_fd(watch);
  flags = dbus_watch_get_flags(watch);

  if (flags & DBUS_WATCH_READABLE)
    cond |= POLLIN;
  if (flags & DBUS_WATCH_WRITABLE)
    cond |= POLLOUT;
  ++max_i;
  pollfds[max_i].fd = fd;
  pollfds[max_i].events = cond;
  watches[max_i] = watch;
  return 1;
}

void _remove_watch(DBusWatch *watch, void *data) {
  int i, found = 0;
  for (i = 0; i <= max_i; ++i) {
    if (watches[i] == watch) {
      found = 1;
      break;
    }
  }
  if (!found) {
    printf("watch %p not found\n", (void *)watch);
    return;
  }
  memset(&pollfds[i], 0, sizeof(pollfds[i]));
  watches[i] = NULL;
  if (i == max_i && max_i > 0)
    --max_i;
}

void _run_loop_dbus(DBusConnection *connection) {
  struct pollfd fds[max_i];
  DBusWatch *watch[max_i];
  int nfds, i;

  for (nfds = i = 0; i <= max_i; ++i) {
    if (pollfds[i].fd == 0 || !dbus_watch_get_enabled(watches[i])) {
      continue;
    }

    fds[nfds].fd = pollfds[i].fd;
    fds[nfds].events = pollfds[i].events;
    fds[nfds].revents = 0;
    watch[nfds] = watches[i];
    ++nfds;
  }

  if (poll(fds, nfds, -1) <= 0) {
    fprintf(stderr, "poll");
    return;
  }

  for (i = 0; i < nfds; ++i) {
    if (fds[i].revents) {
      unsigned int flags = 0;

      if (fds[i].revents & POLLIN)
        flags |= DBUS_WATCH_READABLE;
      if (fds[i].revents & POLLOUT)
        flags |= DBUS_WATCH_WRITABLE;
      if (fds[i].revents & POLLHUP)
        flags |= DBUS_WATCH_HANGUP;
      if (fds[i].revents & POLLERR)
        flags |= DBUS_WATCH_ERROR;

      while (!dbus_watch_handle(watch[i], flags)) {
        printf("dbus_watch_handle needs more memory\n");
        break;
      }

      while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS)
        ;
    }
  }
}
