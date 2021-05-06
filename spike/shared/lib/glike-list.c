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
static list_t *_handle_list_init_empty(list_t *list);

/* Private function ----------------------------------------------------------*/
list_t *list_init(list_t *list, void *data)
{
  if(NULL == list) return list;

  INIT_LIST_HEAD(&list->head);
  list->data = data;
  return list;
}

list_head_t *list_append_node(list_head_t *list_head, list_t *node)
{
  if(NULL == node) {
    goto __end;
  }

  if(NULL == list_head) {
    list_head = list_alloc(NULL);
    if(NULL == list_head) {
      goto __end;
    }
  }

  _handle_list_init_empty(list_head);
  list_add_tail(&node->head, &list_head->head);

__end:
  return list_head;
}

list_head_t *list_prepend_node(list_head_t *list_head, list_t *node)
{
  if(NULL == node) {
    goto __end;
  }

  if(NULL == list_head) {
    list_head = list_alloc(NULL);
    if(NULL == list_head) {
      goto __end;
    }
  }

  _handle_list_init_empty(list_head);
  list_add(&node->head, &list_head->head);

__end:
  return list_head;
}

list_head_t *list_append(list_head_t *list_head, void *data)
{
  list_t *new_node = list_alloc(data);

  list_head = list_append_node(list_head, new_node);
  if(NULL == list_head) {
    list_free(new_node);
  }

  return list_head;
}

list_head_t *list_prepend(list_head_t *list_head, void *data)
{
  list_t *new_node = list_alloc(data);

  list_head = list_prepend_node(list_head, new_node);
  if(NULL == list_head) {
    list_free(new_node);
  }

  return list_head;
}

list_t *list_get_first(list_head_t *list_head)
{
  return list_empty(&list_head->head)
             ? NULL
             : list_first_entry(&list_head->head, list_t, head);
}

list_t *list_get_last(list_head_t *list_head)
{
  return list_empty(&list_head->head)
             ? NULL
             : list_last_entry(&list_head->head, list_t, head);
}

list_t *list_node_remove(list_t *node)
{
  list_del(&node->head);
  return node;
}

void list_node_free_full(list_t *node, DestroyCallback_t free_func)
{
  if(node == NULL) {
    return;
  }

  if(free_func != NULL) {
    free_func(node->data);
  }

  list_free(node);
}

static list_t *_handle_list_init_empty(list_t *list)
{
  if(list && (list->head.next == NULL || list->head.prev == NULL)) {
    list_init(list, list->data);
  }

  return list;
}
