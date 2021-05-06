/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _G_LIKE_LIST_H
#define _G_LIKE_LIST_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdlib.h>

#include "list.h"
#include "typedef.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct list
{
  void *           data;
  struct list_head head;
} list_t;
typedef list_t list_head_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
#define list_alloc(data) list_init(malloc(sizeof(list_t)), data)
#define list_foreach(node, list)                                               \
  if(!is_list_empty(list))                                                     \
    for(__typeof__(*node) *__save__ = NULL; __save__ == NULL;)                 \
  list_for_each_entry_safe(node, __save__, &(list)->head, head)

inline static bool is_list_empty(const list_head_t *list_head)
{
  return (list_head == NULL) || list_empty(&list_head->head) ||
         (list_head->head.prev == NULL && list_head->head.next == NULL);
}

inline static void list_free(list_t *list)
{
  free(list);
}

list_t *list_init(list_t *list, void *data);

list_head_t *list_append_node(list_head_t *list_head, list_t *node);
list_head_t *list_prepend_node(list_head_t *list_head, list_t *node);

list_head_t *list_append(list_head_t *list_head, void *data);
list_head_t *list_prepend(list_head_t *list_head, void *data);

list_t *list_get_first(list_head_t *list_head);
list_t *list_get_last(list_head_t *list_head);

list_t *list_node_remove(list_t *node);
void    list_node_free_full(list_t *node, DestroyCallback_t free_func);

#ifdef __cplusplus
}
#endif
#endif /* _G_LIKE_LIST_H */
