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
struct interface_data
{
  char *                   name;
  const DBusMethodTable *  methods;
  const DBusSignalTable *  signals;
  const DBusPropertyTable *properties;
  sets_t                   pending_prop;
  void *                   user_data;
  DBusDestroyFunction      destroy;
};

struct dbus_object
{
  DBusConnection *    conn;
  char *              path;
  char *              introspect;
  sets_t              interfaces;
  sets_t              added;
  sets_t              removed;
  sets_t              objects;
  struct dbus_object *parent;
  bool                pending_prop;
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void generate_introspection_xml(DBusConnection *    conn,
                                struct dbus_object *data,
                                const char *        path);

void interface_sets_init(sets_t *sets);
bool find_interface_by_name(struct dbus_object *    dbus_object,
                            const char *            name,
                            struct interface_data **interface);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_OBJECT_XML_H */
