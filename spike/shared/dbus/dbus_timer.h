/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_TIMER_H
#define _DBUS_TIMER_H
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
dbus_bool_t add_timeout(DBusTimeout *t, void *data);
void        remove_timeout(DBusTimeout *t, void *data);
void        toggle_timeout(DBusTimeout *t, void *data);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_TIMER_H */
