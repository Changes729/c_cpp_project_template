/** See a brief introduction (right-hand button) */
#include "glike-list.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void _handle_list_init_empty(list_t *list_head);

/* Private function ----------------------------------------------------------*/
bool list_append(list_t *list_head, void *data)
{
  _handle_list_init_empty(list_head);

  assert(list_head != NULL);
  assert(list_find(list_head, data) == NULL);

  list_t *new_list = list_alloc();
  if(new_list != NULL) {
    list_add_tail(&new_list->head, &list_head->head);
    new_list->data = data;
  }

  return new_list != NULL;
}

bool list_remove(list_t *list_head, const void *data)
{
  assert(list_head != NULL);

  list_t *node;
  list_for_each_entry(node, &list_head->head, head)
  {
    if(node->data == data) {
      list_del(&node->head);
      list_free(node);
      node = NULL;
      break;
    }
  }

  return node == NULL;
}

list_t *list_find(list_t *list_head, const void *data)
{
  assert(list_head != NULL);

  list_t *node, *find = NULL;
  list_for_each_entry(node, &list_head->head, head)
  {
    if(node->data == data) {
      find = node;
      break;
    }
  }

  return find;
}

list_t *list_find_custom(list_t *list, const void *data, CompareCallback_t func)
{
  assert(list != NULL);
  assert(func != NULL);

  list_t *node, *find = NULL;
  list_for_each_entry(node, &list->head, head)
  {
    if(func(data, node->data) == 0) {
      find = node;
      break;
    }
  }

  return find;
}

static void _handle_list_init_empty(list_t *list_head)
{
  if(list_head->head.next == NULL || list_head->head.prev == NULL) {
    list_init(list_head);
  }
}
