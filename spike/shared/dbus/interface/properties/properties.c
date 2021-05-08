/** See a brief introduction (right-hand button) */
#include "properties.h"
/* Private include -----------------------------------------------------------*/
#include "dbus-interface-home.h"
#include "dbus-interface-inner.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static DBusMethodTable properties_methods[] = {
    {DBUS_METHOD("Get",
                 DBUS_ARGS({"interface", "s"}, {"name", "s"}),
                 DBUS_ARGS({"value", "v"}),
                 NULL)},
    {DBUS_METHOD("Set",
                 DBUS_ARGS({"interface", "s"}, {"name", "s"}, {"value", "v"}),
                 NULL,
                 NULL)},
    {DBUS_METHOD("GetAll",
                 DBUS_ARGS({"interface", "s"}),
                 DBUS_ARGS({"properties", "a{sv}"}),
                 NULL)},
    {},
};

static const DBusSignalTable properties_signals[] = {
    {DBUS_SIGNAL("PropertiesChanged",
                 DBUS_ARGS({"interface", "s"},
                           {"changed_properties", "a{sv}"},
                           {"invalidated_properties", "as"}))},
    {},
};

/* Private function ----------------------------------------------------------*/
interface_data_t* properties_regist(dbus_object_t* data)
{
  return add_interface(data,
                       DBUS_INTERFACE_PROPERTIES,
                       properties_methods,
                       properties_signals,
                       NULL,
                       data,
                       NULL);
}

void properties_unregist(dbus_object_t* data)
{
  remove_interface(data, DBUS_INTERFACE_PROPERTIES);
}