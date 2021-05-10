/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_OBJECT_XML_H
#define _DBUS_OBJECT_XML_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus_helper.h>

#include "set-list.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct dbus_object
{
  DBusConnection*     conn;
  char*               path;
  char*               introspect;
  sets_t              interfaces;
  sets_t              added;
  sets_t              removed;
  sets_t              objects;
  struct dbus_object* parent;
  bool                pending_prop;
} dbus_object_t;

struct property_data
{
  DBusConnection*        conn;
  DBusPendingPropertySet id;
  DBusMessage*           message;
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
dbus_object_t* attach_dbus_object(DBusConnection* conn, const char* path);
void           detach_dbus_object(DBusConnection* conn, const char* path);

void interface_sets_init(sets_t* sets);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_OBJECT_XML_H */
