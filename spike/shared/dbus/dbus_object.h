/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_XML_H
#define _DBUS_XML_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "dbus_helper.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct dbus_object    dbus_object_t;
typedef struct interface_data interface_data_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void register_dbus_object_path(DBusConnection* conn);
void unregister_dbus_object_path(DBusConnection* conn);

dbus_object_t* attach_dbus_object(DBusConnection* conn, const char* path);
void           detach_dbus_object(DBusConnection* conn, const char* path);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_XML_H */
