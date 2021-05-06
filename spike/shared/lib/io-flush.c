/** See a brief introduction (right-hand button) */
#include "io-flush.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <unistd.h>

#include "list.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct __io_list_head
{
  struct list_head head;
  size_t           count;
} io_list_head_t;

typedef struct
{
  struct list_head node;
  fd_desc_t        pkg;
  fd_callback_t    callback;
  void*            user_data;
  bool             ignore;
} io_node_t;

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static io_list_head_t io_list_head = {
    .head = {&io_list_head.head, &io_list_head.head}};
static int _epoll_fd = -1;

/* Private function prototypes -----------------------------------------------*/
static void           remove_node(io_node_t* node, bool later);
static struct timeval _translate_ms(uint32_t ms);

/* Private function ----------------------------------------------------------*/
bool io_epoll_fd_init()
{
  assert(_epoll_fd == -1);

  _epoll_fd = epoll_create1(0);

  return _epoll_fd != -1;
}

void io_epoll_fd_deinit()
{
  if(_epoll_fd != -1) close(_epoll_fd);
  _epoll_fd = -1;
}

void io_flush_select(uint32_t ms)
{
  struct timeval tv = _translate_ms(ms);
  fd_set         descriptors_read;
  fd_set         descriptors_write;
  fd_set         descriptors_error;
  int            highest_fd = -1;

  FD_ZERO(&descriptors_read);
  FD_ZERO(&descriptors_write);
  FD_ZERO(&descriptors_error);

  // set fds.
  io_node_t* node;
  list_for_each_entry(node, &io_list_head.head, node)
  {
    if(node->pkg.flag & IO_NOTICE_READ) {
      FD_SET(node->pkg.fd, &descriptors_read);
    }

    if(node->pkg.flag & IO_NOTICE_WRITE) {
      FD_SET(node->pkg.fd, &descriptors_write);
    }

    if(node->pkg.flag & IO_NOTICE_ERR) {
      FD_SET(node->pkg.fd, &descriptors_error);
    }

    if(node->pkg.fd > highest_fd) {
      highest_fd = node->pkg.fd;
    }
  }

  // select
  select(highest_fd + 1,
         &descriptors_read,
         &descriptors_write,
         &descriptors_error,
         &tv);

  // map fds.
  list_for_each_entry(node, &io_list_head.head, node)
  {
    fd_desc_t desc = {node->pkg.fd, 0};
    if(node->ignore) continue;
    if(FD_ISSET(node->pkg.fd, &descriptors_read)) desc.flag |= IO_NOTICE_READ;
    if(FD_ISSET(node->pkg.fd, &descriptors_write)) desc.flag |= IO_NOTICE_WRITE;
    if(FD_ISSET(node->pkg.fd, &descriptors_error)) desc.flag |= IO_NOTICE_ERR;

    node->callback(node->user_data, desc);
  }

  io_node_t* remove;
  list_for_each_entry_safe(remove, node, &io_list_head.head, node)
  {
    if(remove->ignore) {
      remove_node(remove, false);
    }
  }
}

void io_flush_poll(uint32_t ms)
{
  struct pollfd fds[io_list_head.count];
  short         cond  = 0;
  size_t        index = 0;

  // set fds.
  io_node_t* node;
  list_for_each_entry(node, &io_list_head.head, node)
  {
    if(node->pkg.flag & IO_NOTICE_READ) cond |= POLLIN;
    if(node->pkg.flag & IO_NOTICE_WRITE) cond |= POLLOUT;
    if(node->pkg.flag & IO_NOTICE_ERR) cond |= POLLERR;
    if(node->pkg.flag & IO_NOTICE_HUP) cond |= POLLHUP;

    fds[index++] = (struct pollfd){node->pkg.fd, cond, 0};
  }

  poll(fds, index, ms);

  // map fds.
  index = 0;
  list_for_each_entry(node, &io_list_head.head, node)
  {
    short flags   = 0;
    short revents = fds[index++].revents;
    if(!node->ignore && revents != 0) {
      if(revents & POLLIN) flags |= IO_NOTICE_READ;
      if(revents & POLLOUT) flags |= IO_NOTICE_WRITE;
      if(revents & POLLERR) flags |= IO_NOTICE_ERR;
      if(revents & POLLHUP) flags |= IO_NOTICE_HUP;

      node->callback(node->user_data, (fd_desc_t){node->pkg.fd, flags});
    }
  }

  io_node_t* remove;
  list_for_each_entry_safe(remove, node, &io_list_head.head, node)
  {
    if(remove->ignore) {
      remove_node(remove, false);
    }
  }
}

void io_flush_epoll(uint32_t ms)
{
  struct epoll_event evs[io_list_head.count];
  int                nfds = epoll_wait(_epoll_fd, evs, io_list_head.count, ms);

  for(size_t i = 0; i < nfds; ++i) {
    io_node_t* node    = evs[i].data.ptr;
    short      flags   = 0;
    uint32_t   revents = evs[i].events;

    if(!node->ignore && revents != 0) {
      if(revents & POLLIN) flags |= IO_NOTICE_READ;
      if(revents & POLLOUT) flags |= IO_NOTICE_WRITE;
      if(revents & POLLERR) flags |= IO_NOTICE_ERR;
      if(revents & POLLHUP) flags |= IO_NOTICE_HUP;

      node->callback(node->user_data, (fd_desc_t){node->pkg.fd, flags});
    }
  }

  io_node_t *node, *remove;
  list_for_each_entry_safe(remove, node, &io_list_head.head, node)
  {
    if(remove->ignore) {
      remove_node(remove, false);
    }
  }
}

bool io_notice_file(fd_desc_t pkg, fd_callback_t callback, void* user_data)
{
  assert(callback != NULL);

  io_node_t* node = malloc(sizeof(io_node_t));
  node->pkg       = pkg;
  node->callback  = callback;
  node->user_data = user_data;
  node->ignore    = false;
  list_add_tail(&node->node, &io_list_head.head);
  io_list_head.count++;

  if(_epoll_fd != -1) {
    uint32_t cond = 0;
    if(node->pkg.flag & IO_NOTICE_READ) cond |= POLLIN;
    if(node->pkg.flag & IO_NOTICE_WRITE) cond |= POLLOUT;
    if(node->pkg.flag & IO_NOTICE_ERR) cond |= POLLERR;
    if(node->pkg.flag & IO_NOTICE_HUP) cond |= POLLHUP;

    struct epoll_event data = {.events = cond, .data = {.ptr = node}};
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, node->pkg.fd, &data);
  }
}

void io_ignore_file(int fd)
{
  io_node_t* node;
  list_for_each_entry(node, &io_list_head.head, node)
  {
    if(node->pkg.fd == fd) {
      remove_node(node, true);
      break;
    }
  }
}

static void remove_node(io_node_t* node, bool later)
{
  if(later) {
    node->ignore = true;
  } else {
    if(_epoll_fd != -1) {
      epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, node->pkg.fd, NULL);
    }

    list_del(&node->node);
    free(node);
    io_list_head.count--;
  }
}

static struct timeval _translate_ms(uint32_t ms)
{
  return (struct timeval){ms / 1000, (ms - ms / 1000) * 1000};
}
