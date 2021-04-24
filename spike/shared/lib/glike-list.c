/** See a brief introduction (right-hand button) */
#include "glike-list.h"
/* Private include -----------------------------------------------------------*/
#include <stdlib.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
bool list_append(list_t *list_head, void *data)
{
  list_t *new_list = list_alloc();

  if(new_list != NULL) {
    list_add_tail(&new_list->head, &list_head->head);
    new_list->data = data;
  }

  return new_list != NULL;
}

bool list_remove(list_t *list_head, const void *data)
{
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
