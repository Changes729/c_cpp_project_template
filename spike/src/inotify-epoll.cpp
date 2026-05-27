#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define BUF_LEN    (1024 * (sizeof(struct inotify_event) + 16))

int main()
{
  int         fds[2];
  const char *path[2] = {
      ".",
      "..",
  };

  int index = 0;
  for(auto &fd: fds) {
    fd = inotify_init1(IN_NONBLOCK);
    if(fd < 0) {
      perror("inotify_init1");
      exit(EXIT_FAILURE);
    }

    int wd = inotify_add_watch(fd, path[index], IN_CREATE | IN_DELETE);
    if(wd == -1) {
      perror("inotify_add_watch");
      exit(EXIT_FAILURE);
    }

    index += 1;
  }

  int                epfd = epoll_create1(0);
  struct epoll_event events[MAX_EVENTS];
  for(auto &fd: fds) {
    struct epoll_event ev;
    ev.events  = EPOLLIN;  // Watch for input (ready to read)
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
  }

  while(1) {
    // 4. Wait for events (blocks until one occurs)
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for(int n = 0; n < nfds; ++n) {
      for(auto &fd: fds) {
        if(events[n].data.fd == fd) {
          char    buffer[BUF_LEN];
          ssize_t len = read(fd, buffer, BUF_LEN);

          // Process inotify events from the buffer
          for(char *ptr = buffer; ptr < buffer + len;) {
            struct inotify_event *event = (struct inotify_event *)ptr;
            if(event->len) {
              if(event->mask & IN_CREATE) printf("Created: %s\n", event->name);
              if(event->mask & IN_DELETE) printf("Deleted: %s\n", event->name);
            }
            ptr += sizeof(struct inotify_event) + event->len;
          }
        }
      }
    }
  }
}