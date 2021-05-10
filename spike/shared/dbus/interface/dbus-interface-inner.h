/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_INTERFACE_INNER_H
#define _DBUS_INTERFACE_INNER_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus/dbus.h>

#include "dbus_helper.h"
#include "dbus_object_helper.h"
#include "set-list.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct interface_data
{
  char*                    name;
  const DBusMethodTable*   methods;
  const DBusSignalTable*   signals;
  const DBusPropertyTable* properties;
  sets_t                   pending_prop;
  void*                    user_data;
  DBusDestroyFunction      destroy;
} interface_data_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
interface_data_t* add_interface(dbus_object_t*           data,
                                const char*              name,
                                const DBusMethodTable*   methods,
                                const DBusSignalTable*   signals,
                                const DBusPropertyTable* properties,
                                void*                    user_data,
                                DBusDestroyFunction      destroy);

void remove_interface(dbus_object_t* data, const char* name);
bool find_interface_by_name(struct dbus_object*     dbus_object,
                            const char*             name,
                            struct interface_data** interface);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_INTERFACE_INNER_H */
