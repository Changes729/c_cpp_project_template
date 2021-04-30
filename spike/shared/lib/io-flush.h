/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IO_FLUSH_H
#define _IO_FLUSH_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdbool.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef enum io_notice_type {
  IO_NOTICE_READ  = 1 << 0,
  IO_NOTICE_WRITE = 1 << 1,
  IO_NOTICE_ERR   = 1 << 2,
  IO_NOTICE_HUP   = 1 << 3,
} io_notice_type;

/* Public template -----------------------------------------------------------*/
typedef struct
{
  int   fd;
  short flag;
} fd_desc_t;

typedef void (*fd_callback_t)(void* user_data, fd_desc_t pkg);

/* Public function prototypes ------------------------------------------------*/
void io_flush_select(/*timeout*/);
void io_flush_poll(/*timeout*/);
void io_flush_epoll(/*timeout*/);

bool io_notice_file(fd_desc_t pkg, fd_callback_t callback, void* user_data);
void io_ignore_file(int fd);

#ifdef __cplusplus
}
#endif
#endif /* _IO_FOCUS_H */
