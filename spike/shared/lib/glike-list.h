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

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
inline static list_t *list_alloc(void)
{
  list_t *list = malloc(sizeof(list_t));
  INIT_LIST_HEAD(&list->head);
  list->data = NULL;
  return list;
}

inline static void list_free(list_t *list)
{
  free(list);
}

bool    list_append(list_t *list_head, void *data);
bool    list_remove(list_t *list_head, const void *data);
list_t *list_find(list_t *list, const void *data);

void    g_slist_free_full(list_t *list, DestroyCallback_t free_func);
list_t *g_slist_prepend(list_t *list, void *data);
list_t *g_slist_insert(list_t *list, void *data, int position);
list_t *g_slist_insert_sorted(list_t *list, void *data, CompareCallback_t func);
list_t *g_slist_insert_sorted_with_data(list_t *              list,
                                        void *                data,
                                        CompareCallbackFull_t func,
                                        void *                user_data);
list_t *g_slist_insert_before(list_t *slist, list_t *sibling, void *data);
list_t *g_slist_concat(list_t *list1, list_t *list2);
list_t *g_slist_remove_all(list_t *list, const void *data);
list_t *g_slist_remove_link(list_t *list, list_t *link_);
list_t *g_slist_delete_link(list_t *list, list_t *link_);
list_t *g_slist_reverse(list_t *list);
list_t *g_slist_copy(list_t *list);

list_t *g_slist_copy_deep(list_t *list, CopyCallback_t func, void *user_data);
list_t *g_slist_nth(list_t *list, unsigned n);
list_t *
    g_slist_find_custom(list_t *list, const void *data, CompareCallback_t func);
int g_slist_position(list_t *list, list_t *llink);
int g_slist_index(list_t *list, const void *data);
list_t * g_slist_last(list_t *list);
unsigned g_slist_length(list_t *list);
void     g_slist_foreach(list_t *list, Function_t func, void *user_data);
list_t * g_slist_sort(list_t *list, CompareCallback_t compare_func);
list_t * g_slist_sort_with_data(list_t *              list,
                                CompareCallbackFull_t compare_func,
                                void *                user_data);
void *   g_slist_nth_data(list_t *list, unsigned n);

void g_clear_slist(list_t **slist_ptr, DestroyCallback_t destroy);

#define g_clear_slist(slist_ptr, destroy)                                      \
  G_STMT_START                                                                 \
  {                                                                            \
    list_t *_slist;                                                            \
                                                                               \
    _slist = *(slist_ptr);                                                     \
    if(_slist) {                                                               \
      *slist_ptr = NULL;                                                       \
                                                                               \
      if((destroy) != NULL)                                                    \
        g_slist_free_full(_slist, (destroy));                                  \
      else                                                                     \
        g_slist_free(_slist);                                                  \
    }                                                                          \
  }                                                                            \
  G_STMT_END                                                                   \
  GLIB_AVAILABLE_MACRO_IN_2_64

#define g_slist_next(slist) ((slist) ? (((list_t *)(slist))->next) : NULL)

#ifdef __cplusplus
}
#endif
#endif /* _G_LIKE_LIST_H */
