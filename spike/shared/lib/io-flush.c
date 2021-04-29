/** See a brief introduction (right-hand button) */
#include "io-flush.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

#include "list.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
struct __io_list_head
{
  struct list_head head;
  size_t           count;
} io_list_head = {.head = {&io_list_head.head, &io_list_head.head}};

typedef struct
{
  struct list_head node;
  int              fd;
  short            io_focus_type;
} io_node_t;

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void io_flush_select(/*timeout*/) {}

void io_flush_poll(/*timeout*/) {}

void io_flush_epoll(/*timeout*/) {}

bool io_notice_file(int fd, short io_focus_type)
{
  io_node_t* node     = malloc(sizeof(io_node_t));
  node->fd            = fd;
  node->io_focus_type = io_focus_type;
  list_add_tail(&node->node, &io_list_head.head);
}

void io_ignore_file(int fd)
{
  io_node_t* node;
  list_for_each_entry(node, &io_list_head.head, node)
  {
    if(node->fd == fd) {
      list_del(&node->node);
      free(node);
      break;
    }
  }
}
