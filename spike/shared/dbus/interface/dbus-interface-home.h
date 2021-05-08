/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_INTERFACE_HOME_H
#define _DBUS_INTERFACE_HOME_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "dbus_object.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef uintptr_t dbus_interface_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
bool attach_interface(struct dbus_object*, const char* interface_name);
void detach_interface(struct dbus_object*, const char* interface_name);

bool find_interface_method(struct dbus_object* dbus_object,
                           const char*         iface_name,
                           const char*         method_name,
                           DBusMethodFunction* method);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_INTERFACE_HOME_H */
