/** See a brief introduction (right-hand button) */
#include "dbus_object.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbus_define.h"
#include "dbus_error.h"
#include "dbus_object_helper.h"
#include "glike-list.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DBusHandlerResult
server_message_handler(DBusConnection *, DBusMessage *, void *user_data);
static DBusMethodFunction
find_interface_method(DBusMessage *, struct dbus_object *);

static DBusMessage *introspect(DBusConnection *, DBusMessage *, void *user_data);

/* Private variables ---------------------------------------------------------*/
static DBusMethodTable introspect_methods[] = {
    {DBUS_METHOD("Introspect", NULL, DBUS_ARGS({"xml", "s"}), introspect)},
    {},
};

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

static DBusMethodTable object_methods[] = {
    {DBUS_METHOD("GetManagedObjects",
                 NULL,
                 DBUS_ARGS({"objects", "a{oa{sa{sv}}}"}),
                 NULL)},
    {},
};

static const DBusSignalTable object_signals[] = {
    {DBUS_SIGNAL("InterfacesAdded",
                 DBUS_ARGS({"object", "o"}, {"interfaces", "a{sa{sv}}"}))},
    {DBUS_SIGNAL("InterfacesRemoved",
                 DBUS_ARGS({"object", "o"}, {"interfaces", "as"}))},
    {},
};

static struct interface_data interface_introspectable = {
    .name       = DBUS_INTERFACE_INTROSPECTABLE,
    .methods    = introspect_methods,
    .signals    = NULL,
    .properties = NULL,
    .user_data  = NULL,
};

static struct interface_data interface_properties = {
    .name       = DBUS_INTERFACE_PROPERTIES,
    .methods    = properties_methods,
    .signals    = properties_signals,
    .properties = NULL,
    .user_data  = NULL,
};

static struct interface_data object_properties = {
    .name       = DBUS_INTERFACE_PROPERTIES,
    .methods    = object_methods,
    .signals    = object_signals,
    .properties = NULL,
    .user_data  = NULL,
};

static const DBusObjectPathVTable server_vtable = {
    .message_function = server_message_handler,
};

static struct dbus_object dbus_object = {
    .path = "/",
};

/* Private class -------------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void register_dbus_object_path(DBusConnection *conn)
{
  sets_add(&dbus_object.interfaces, &interface_introspectable);
  sets_add(&dbus_object.interfaces, &interface_properties);
  sets_add(&dbus_object.interfaces, &object_properties);

  dbus_connection_register_object_path(conn,
                                       dbus_object.path,
                                       &server_vtable,
                                       &dbus_object);
}

void unregister_dbus_object_path(DBusConnection *conn)
{
  dbus_connection_unregister_object_path(conn, dbus_object.path);

  sets_remove(&dbus_object.interfaces, &object_properties);
  sets_remove(&dbus_object.interfaces, &interface_properties);
  sets_remove(&dbus_object.interfaces, &interface_introspectable);
  free(dbus_object.introspect);
  dbus_object.introspect = NULL;
}

static DBusHandlerResult
server_message_handler(DBusConnection *conn, DBusMessage *message, void *data)
{
  struct dbus_object *dbus_object = data;

  DBusHandlerResult  result   = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  DBusMessage *      reply    = NULL;
  DBusMethodFunction function = NULL;
  DBusError          err;

  printf("Got D-Bus request: %s.%s on %s\n",
         dbus_message_get_interface(message),
         dbus_message_get_member(message),
         dbus_message_get_path(message));

  do {
    if(strcmp(dbus_message_get_path(message), dbus_object->path) != 0) {
      break;
    }

    dbus_error_init(&err);

    result   = DBUS_HANDLER_RESULT_HANDLED;
    function = find_interface_method(message, dbus_object);
    if(NULL == function) {
      dbus_set_error_const(&err, ERROR_INTERFACE ".Failed", "no such method.");
    } else {
      reply = function(conn, message, data);
    }

    if(dbus_error_is_set(&err)) {
      if(reply) dbus_message_unref(reply);
      reply = dbus_message_new_error(message, err.name, err.message);
      dbus_error_free(&err);
    }

    if(NULL == reply) {
      result = DBUS_HANDLER_RESULT_NEED_MEMORY;
      break;
    }

    if(!dbus_connection_send(conn, reply, NULL)) {
      result = DBUS_HANDLER_RESULT_NEED_MEMORY;
    }

    dbus_message_unref(reply);

  } while(0);

  return result;
}

struct interface_find_method_by_name_pkg
{
  const char *       iface_name;
  DBusMessage *      message;
  DBusMethodFunction method;
};

int interface_find_method_by_name(void *data, void *user_data)
{
  struct interface_find_method_by_name_pkg *pkg       = user_data;
  struct interface_data *                   interface = data;

  if(strcmp(interface->name, pkg->iface_name) != 0) {
    return true;
  }

  for(const DBusMethodTable *node = interface->methods; NULL != node->name; node++)
  {
    if(dbus_message_is_method_call(pkg->message, interface->name, node->name)) {
      pkg->method = node->function;
      return false;
    }
  }

  return true;
}

static DBusMethodFunction
find_interface_method(DBusMessage *message, struct dbus_object *dbus_object)
{
  const char *interface_name = dbus_message_get_interface(message);
  struct interface_find_method_by_name_pkg pkg = {interface_name, message, NULL};

  sets_foreach(&dbus_object->interfaces, interface_find_method_by_name, &pkg);

  return pkg.method;
}

static DBusMessage *
introspect(DBusConnection *connection, DBusMessage *message, void *user_data)
{
  struct dbus_object *data  = user_data;
  DBusMessage *       reply = NULL;

  reply = dbus_message_new_method_return(message);
  if(reply == NULL) {
    goto end;
  }

  if(NULL == data->introspect) {
    generate_introspection_xml(connection, data, dbus_message_get_path(message));
  }

  if(NULL != data->introspect) {
    dbus_message_append_args(reply,
                             DBUS_TYPE_STRING,
                             &data->introspect,
                             DBUS_TYPE_INVALID);
  } else {
    dbus_message_unref(reply);
    reply = NULL;
  }

end:
  return reply;
}
