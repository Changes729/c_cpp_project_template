/** See a brief introduction (right-hand button) */
#include "timer-task.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>
#include <time.h>

#include "glike-list.h"
#include "task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifndef CLOCK_MONOTONIC
  #define CLOCK_MONOTONIC 1
#endif

/* Private typedef -----------------------------------------------------------*/
struct _timer_task
{
  task_t          task;
  struct timespec tend;
  uint32_t        alarm;

  bool remove;
};

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
list_head_t _timer_tasks;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static struct timespec _calculate_end_time(uint32_t ms);
static double          _cmp_timespec(struct timespec* a, struct timespec* b);
static void            _timer_after_loop();

/* Private function ----------------------------------------------------------*/
timer_task_t* timer_task_new(uint32_t ms, operation_cb_t operation, void* user_data)
{
  struct _timer_task* task = malloc(sizeof(*task));
  if(task != NULL) {
    task->task   = (task_t){.data = user_data, .operation = operation};
    task->alarm  = ms;
    task->remove = false;
    task->tend   = _calculate_end_time(ms);

    // find a later one and insert.
    list_t* find = NULL;
    list_foreach_r(list_node, &_timer_tasks)
    {
      struct _timer_task* find_task = list_node->data;
      if(_cmp_timespec(&find_task->tend, &task->tend) > 0) {
        find = list_node;
        break;
      }
    }

    if(list_prepend(&_timer_tasks, task, find) == NULL) {
      free(task);
      task = NULL;
    }
  }
  return task;
}

void timer_task_del(timer_task_t* task)
{
  task->remove = true;
}

void timer_flush()
{
  struct timespec tnow;

  clock_gettime(CLOCK_MONOTONIC, &tnow);

  struct _timer_task* task;
  list_foreach_data(task, &_timer_tasks)
  {
    if(!task->remove && _cmp_timespec(&tnow, &task->tend) >= 0) {
      task_run(task->task);
      task->remove = true;
    }
  }

  _timer_after_loop();
}

uint32_t timer_next_alarm()
{
  uint32_t        ms = UINT32_MAX;
  struct timespec tnow;

  clock_gettime(CLOCK_MONOTONIC, &tnow);

  struct _timer_task* task;
  list_foreach_data(task, &_timer_tasks)
  {
    double time = _cmp_timespec(&task->tend, &tnow);
    ms          = (time < 0) ? 0 : ((uint32_t)(time) + 1);
    break;
  }

  return ms;
}

static struct timespec _calculate_end_time(uint32_t ms)
{
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);

  double ns   = ms * 1e6;
  double curr = time.tv_sec * 1e9 + time.tv_nsec + ns;

  time.tv_sec  = curr * 1e-9;
  time.tv_nsec = curr - time.tv_sec * 1e9;

  return time;
}

static double _cmp_timespec(struct timespec* a, struct timespec* b)
{
  double ta = a->tv_sec * 1e9 + a->tv_nsec;
  double tb = b->tv_sec * 1e9 + b->tv_nsec;

  return (ta - tb) * 1e-6;
}

static void _timer_after_loop()
{
  struct _timer_task* task;
  bool                remove = false;
  list_foreach(list_node, &_timer_tasks)
  {
    task = list_node->data;
    if(remove) {
      free(list_node_remove(list_get_prev(&_timer_tasks, list_node)));
    }

    remove = task->remove;
  }

  if(remove) {
    free(list_node_remove(list_get_last(&_timer_tasks)));
  }
}
