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
  struct timespec tstart;
  uint32_t        alarm;

  bool ignore;
};

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
list_head_t _timer_tasks;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
timer_task_t* timer_task_new(uint32_t ms, Operation_t operation, void* user_data)
{
  struct _timer_task* task = malloc(sizeof(*task));
  if(task != NULL) {
    task->task   = (task_t){.data = user_data, .operation = operation};
    task->alarm  = ms;
    task->ignore = false;
    clock_gettime(CLOCK_MONOTONIC, &task->tstart);
    list_append(&_timer_tasks, task);
  }
  return task;
}

void timer_task_del(timer_task_t* task)
{
  task->ignore = true;
}

void timer_flush()
{
  struct timespec tnow;

  clock_gettime(CLOCK_MONOTONIC, &tnow);

  list_t* node;
  list_foreach(node, &_timer_tasks)
  {
    struct _timer_task* task = node->data;
    uint32_t            diff = (tnow.tv_sec - task->tstart.tv_sec) * 1000 +
                    (tnow.tv_nsec - task->tstart.tv_nsec) / 1000000;

    if(!task->ignore && diff >= task->alarm) {
      task_run(task->task);
      task->ignore = true;
    }
  }

  list_foreach(node, &_timer_tasks)
  {
    struct _timer_task* task = node->data;
    if(task->ignore) {
      list_free(list_node_remove(node));
      free(task);
    }
  }
}

uint32_t timer_next_alarm()
{
  uint32_t        ms = UINT32_MAX;
  struct timespec tnow;

  clock_gettime(CLOCK_MONOTONIC, &tnow);

  list_t* node;
  list_foreach(node, &_timer_tasks)
  {
    struct _timer_task* task = node->data;
    uint32_t            diff = (tnow.tv_sec - task->tstart.tv_sec) * 1000 +
                    (tnow.tv_nsec - task->tstart.tv_nsec) / 1000000;

    if(diff >= task->alarm) {
      ms = 0;
      break;
    } else if(ms > task->alarm - diff) {
      ms = task->alarm - diff;
    }
  }

  return ms;
}
