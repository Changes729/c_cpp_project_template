/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PROPERTIES_H
#define _PROPERTIES_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "dbus-interface-inner.h"
#include "dbus_object_helper.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
interface_data_t* properties_regist(dbus_object_t* data);
void              properties_unregist(dbus_object_t* data);

#ifdef __cplusplus
}
#endif
#endif /* _PROPERTIES_H */
