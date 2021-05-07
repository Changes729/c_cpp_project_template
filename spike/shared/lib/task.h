/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TASK_H
#define _TASK_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "typedef.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct _task
{
  void*          data;
  operation_cb_t operation;
} task_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
static inline void task_run(task_t task)
{
  return task.operation(task.data);
}

#ifdef __cplusplus
}
#endif
#endif /* _TASK_H */
