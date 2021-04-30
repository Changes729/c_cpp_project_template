/** See a brief introduction (right-hand button) */
#include "io-flush.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <sys/select.h>

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
io_list_head_t io_list_head = {.head = {&io_list_head.head, &io_list_head.head}};

/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void io_flush_select(/*timeout*/)
{
  fd_set descriptors_read;
  fd_set descriptors_write;
  fd_set descriptors_error;
  int    highest_fd = -1;

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
         NULL);

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
      list_del(&remove->node);
      free(remove);
    }
  }
}

void io_flush_poll(/*timeout*/) {}

void io_flush_epoll(/*timeout*/) {}

bool io_notice_file(fd_desc_t pkg, fd_callback_t callback, void* user_data)
{
  assert(callback != NULL);

  io_node_t* node = malloc(sizeof(io_node_t));
  node->pkg       = pkg;
  node->callback  = callback;
  node->user_data = user_data;
  node->ignore    = false;
  list_add_tail(&node->node, &io_list_head.head);
}

void io_ignore_file(int fd)
{
  io_node_t* node;
  list_for_each_entry(node, &io_list_head.head, node)
  {
    if(node->pkg.fd == fd) {
      node->ignore = true;
      break;
    }
  }
}
