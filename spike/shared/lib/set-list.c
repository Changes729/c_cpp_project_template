/** See a brief introduction (right-hand button) */
#include "set-list.h"
/* Private include -----------------------------------------------------------*/
/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int inline _point_cmp(const void* a, const void* b)
{
  return (uintptr_t)a - (uintptr_t)b;
}

/* Private function ----------------------------------------------------------*/
sets_t* sets_alloc(CompareCallback_t cmp, DestroyCallback_t destroy)
{
  return sets_init(malloc(sizeof(sets_t)), cmp, destroy);
}

void sets_free(sets_t* set)
{
  free(set);
}

sets_t* sets_init(sets_t* set, CompareCallback_t cmp, DestroyCallback_t destroy)
{
  if(set == NULL) return set;

  list_init(&set->list_head, set);
  set->count      = 0;
  set->cb_cmp     = cmp;
  set->cb_destory = destroy;

  return set;
}

sets_t* sets_add(sets_t* set, void* data)
{
  if(set == NULL) set = sets_alloc(NULL, NULL);
  if(set == NULL) return set;

  if(!sets_find(set, data, NULL)) {
    list_append(&set->list_head, data);
    if(list_get_last(&set->list_head)->data == data) {
      set->count++;
    }
  }

  return set;
}

void sets_remove(sets_t* set, void* data)
{
  CompareCallback_t cb     = (set->cb_cmp == NULL ? _point_cmp : set->cb_cmp);
  list_t*           remove = NULL;

  list_t* node;
  list_foreach(node, &set->list_head)
  {
    if(cb(data, node->data) == 0) {
      remove = node;
      break;
    }
  }

  if(remove != NULL) {
    list_node_free_full(list_node_remove(node), set->cb_destory);
    set->count--;
  }
}

void sets_cleanup(sets_t* set)
{
  list_t* node = NULL;
  while(NULL != (node = list_get_first(&set->list_head))) {
    list_node_free_full(list_node_remove(node), set->cb_destory);
    set->count--;
  }
}

bool sets_find(const sets_t* set, const void* data, void** out)
{
  bool              find = false;
  CompareCallback_t cb   = (set->cb_cmp == NULL ? _point_cmp : set->cb_cmp);

  list_t* node;
  list_foreach(node, &set->list_head)
  {
    if(cb(data, node->data) == 0) {
      if(out != NULL) {
        *out = node->data;
      }
      find = true;
      break;
    }
  }

  return find;
}

unsigned sets_length(const sets_t* set)
{
  return set == NULL ? 0 : set->count;
}

void sets_foreach(sets_t* set, Function_t callback, void* userdata)
{
  list_t* node;
  list_foreach(node, &set->list_head)
  {
    if(!callback(node->data, userdata)) {
      break;
    }
  }
}

list_head_t* sets_get_listhead(sets_t* sets)
{
  return &sets->list_head;
}
