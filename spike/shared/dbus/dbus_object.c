/** See a brief introduction (right-hand button) */
#include "dbus_object.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbus_define.h"
#include "dbus_object_helper.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DBUS_INTERFACE_OBJECT_MANAGER "org.freedesktop.DBus.ObjectManager"

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DBusHandlerResult
server_message_handler(DBusConnection *, DBusMessage *, void *user_data);

static DBusMessage *introspect(DBusConnection *, DBusMessage *, void *user_data);

/* Private variables ---------------------------------------------------------*/
static DBusMethodTable introspect_methods[] = {
    {DBus_METHOD("Introspect", NULL, DBus_ARGS({"xml", "s"}), introspect)},
    {},
};

static DBusMethodTable properties_methods[] __attribute__((unused)) = {
    {DBus_METHOD("Get",
                  DBus_ARGS({"interface", "s"}, {"name", "s"}),
                  DBus_ARGS({"value", "v"}),
                  NULL)},
    {DBus_METHOD("Set",
                  DBus_ARGS({"interface", "s"}, {"name", "s"}, {"value", "v"}),
                  NULL,
                  NULL)},
    {DBus_METHOD("GetAll",
                  DBus_ARGS({"interface", "s"}),
                  DBus_ARGS({"properties", "a{sv}"}),
                  NULL)},
    {},
};

static DBusMethodTable object_methods[] __attribute__((
    unused)) = {{DBus_METHOD("GetManagedObjects",
                              NULL,
                              DBus_ARGS({"objects", "a{oa{sa{sv}}}"}),
                              NULL)},
                {}};

static const DBusObjectPathVTable server_vtable = {
    .message_function = server_message_handler,
};

static struct interface_data interface_data = {
    .name       = DBUS_INTERFACE_INTROSPECTABLE,
    .methods    = introspect_methods,
    .signals    = NULL,
    .properties = NULL,
    .user_data  = NULL,
};

static struct generic_data generic_data = {
    .path       = "/",
    .interfaces = {&generic_data.interfaces.head, &generic_data.interfaces.head},
    .introspect = NULL,
};

/* Private class -------------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void register_dbus_object_path(DBusConnection *conn)
{
  // FIXME:
  struct list *list = malloc(sizeof(struct list));
  INIT_LIST_HEAD(&list->head);

  list->data = &interface_data;

  list_add_tail(&generic_data.interfaces.head, &list->head);

  dbus_connection_register_object_path(conn, "/", &server_vtable, &generic_data);
}

static DBusHandlerResult
server_message_handler(DBusConnection *conn, DBusMessage *message, void *data)
{
  struct generic_data *generic_data = data;

  DBusHandlerResult result;

  printf("Got D-Bus request: %s.%s on %s\n",
         dbus_message_get_interface(message),
         dbus_message_get_member(message),
         dbus_message_get_path(message));

  if(list_empty(&generic_data->interfaces.head)) {
    result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  } else {
    DBusMessage *reply = NULL;
    DBusError    err;

    dbus_error_init(&err);

    struct list *list, *next;
    list_for_each_entry_safe(list, next, &generic_data->interfaces.head, head)
    {
      struct interface_data *interface = list->data;
      DBusMethodFunction    function  = NULL;
      for(const DBusMethodTable *node = interface->methods; NULL != node->name;
          node++)
      {
        if(dbus_message_is_method_call(message, interface->name, node->name)) {
          function = node->function;
          break;
        }
      }

      if(function != NULL) {
        reply = function(conn, message, data);
        break;
      }
    }

    if(dbus_error_is_set(&err)) {
      if(reply) dbus_message_unref(reply);
      reply = dbus_message_new_error(message, err.name, err.message);
      dbus_error_free(&err);
    }

    if(!reply) return DBUS_HANDLER_RESULT_NEED_MEMORY;

    result = DBUS_HANDLER_RESULT_HANDLED;
    if(!dbus_connection_send(conn, reply, NULL))
      result = DBUS_HANDLER_RESULT_NEED_MEMORY;
    dbus_message_unref(reply);
  }

  return result;
}

static DBusMessage *
introspect(DBusConnection *connection, DBusMessage *message, void *user_data)
{
  struct generic_data *data  = user_data;
  DBusMessage *        reply = NULL;

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
