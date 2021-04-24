/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_OBJECT_XML_H
#define _DBUS_OBJECT_XML_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus_helper.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
// FIXME:
struct list
{
  struct list_head head;
  void *           data;
};

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
  char *                    name;
  const DBusMethodTable *   methods;
  const DBusSignalTable *  signals;
  const DBusPropertyTable *properties;
  // GSList *pending_prop;
  void *user_data;
  // DBusDestroyFunction destroy;
};

// FIXME:
struct generic_data
{
  // unsigned int refcount;
  // DBusConnection *conn;
  char *      path;
  struct list interfaces;
  // GSList *objects;
  // GSList *added;
  // GSList *removed;
  // guint process_id;
  // gboolean pending_prop;
  char *introspect;
  // struct generic_data *parent;
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void generate_introspection_xml(DBusConnection *     conn,
                                struct generic_data *data,
                                const char *         path);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_OBJECT_XML_H */
