/** See a brief introduction (right-hand button) */
#include "root-object.h"
/* Private include -----------------------------------------------------------*/
#include "dbus-interface-home.h"
#include "dbus_define.h"
#include "dbus_object_helper.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct dbus_object root = {
    .path = "/",
};

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
int register_root_object(DBusConnection *connection)
{
  struct dbus_object *data = attach_dbus_object(connection, root.path);
  if(data == NULL) return FALSE;

  attach_interface(data, DBUS_INTERFACE_INTROSPECTABLE);
  attach_interface(data, DBUS_INTERFACE_OBJECT_MANAGER);

  return TRUE;
}

void unregister_root_object(DBusConnection *connection)
{
  dbus_object_t *dbus_object;
  if(!dbus_connection_get_object_path_data(connection,
                                           root.path,
                                           (void *)&dbus_object) ||
     dbus_object == NULL)
  {
    return;
  }

  detach_interface(dbus_object, DBUS_INTERFACE_INTROSPECTABLE);
  detach_interface(dbus_object, DBUS_INTERFACE_OBJECT_MANAGER);
  detach_dbus_object(connection, root.path);
}