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
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void register_dbus_object_path(DBusConnection* conn);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_XML_H */
