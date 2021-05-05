/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_EPOLL_H
#define _DBUS_EPOLL_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus/dbus.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
dbus_bool_t _add_watch(DBusWatch *, void *data);
void        _remove_watch(DBusWatch *, void *data);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_EPOLL_H */
