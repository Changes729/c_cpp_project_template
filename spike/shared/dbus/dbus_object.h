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
#define DBUS_OBJECT_INTERFACE

/* Public typedef ------------------------------------------------------------*/
typedef struct __dbus_object dbus_object_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
bool register_dbus_object_path(DBusConnection* conn);
bool unregister_dbus_object_path(DBusConnection* conn);

dbus_object_t* attach_dbus_object(DBusConnection*, const char* path);
dbus_object_t* detach_dbus_object(DBusConnection*, const char* path);

bool add_interface(dbus_object_t* obj,
                   const char*    iface_name,

                   const DBusMethodTable*   methods,
                   const DBusSignalTable*   signals,
                   const DBusPropertyTable* properties,

                   void*               destroy_user_data,
                   DBusDestroyFunction destroy);
bool remove_interface(dbus_object_t* obj, const char* iface_name);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_XML_H */
