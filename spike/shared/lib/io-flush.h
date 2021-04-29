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
  IO_NOTICE_WRITE = 2 << 0,
  IO_NOTICE_ERR   = 3 << 0,
  IO_NOTICE_HUP   = 4 << 0,
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void io_flush_select(/*timeout*/);
void io_flush_poll(/*timeout*/);
void io_flush_epoll(/*timeout*/);

bool io_notice_file(int fd, short io_focus_type);
void io_ignore_file(int fd);

#ifdef __cplusplus
}
#endif
#endif /* _IO_FOCUS_H */
