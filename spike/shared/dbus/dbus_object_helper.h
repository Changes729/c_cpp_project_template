/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_OBJECT_XML_H
#define _DBUS_OBJECT_XML_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus_helper.h>

#include "glike-list.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
// FIXME:
struct string
{
  char * buffer;
  size_t length;
  size_t buffer_size;
};

// FIXME:
struct interface_data
{
  char *                   name;
  const DBusMethodTable *  methods;
  const DBusSignalTable *  signals;
  const DBusPropertyTable *properties;
  list_t *                 pending_prop;  // for properties
  void *                   user_data;
  DBusDestroyFunction      destroy;
};

// FIXME:
struct __dbus_object
{
  DBusConnection *      conn;
  char *                path;
  list_t                interfaces;
  list_t                objects;
  list_t                added;
  list_t                removed;
  bool                  pending_prop;  // for interface add/remove
  char *                introspect;
  struct __dbus_object *parent;
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void generate_introspection_xml(DBusConnection *      conn,
                                struct __dbus_object *data,
                                const char *          path);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_OBJECT_XML_H */
