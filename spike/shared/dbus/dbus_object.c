/** See a brief introduction (right-hand button) */
#include "dbus_object.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbus-interface-home.h"
#include "dbus_define.h"
#include "dbus_error.h"
#include "dbus_object_helper.h"
#include "glike-list.h"
#include "timer-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DBusHandlerResult server_message_handler(DBusConnection *,
                                                DBusMessage *,
                                                void *user_data);

static struct dbus_object *invalidate_parent_data(DBusConnection *conn,
                                                  const char *    child_path);

/* Private variables ---------------------------------------------------------*/
static const DBusObjectPathVTable server_vtable = {
    .message_function = server_message_handler,
};

static struct dbus_object root = {
    .path = "/",
};

/* Private class -------------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void register_dbus_object_path(DBusConnection *conn)
{
  struct dbus_object *data = attach_dbus_object(conn, root.path);
  if(data == NULL) return;

  attach_interface(data, DBUS_INTERFACE_INTROSPECTABLE);
  attach_interface(data, DBUS_INTERFACE_OBJECT_MANAGER);
  // attach_interface(data, DBUS_INTERFACE_PROPERTIES);

  data = attach_dbus_object(conn, "/org/bluez");
  attach_interface(data, DBUS_INTERFACE_INTROSPECTABLE);
  data = attach_dbus_object(conn, "/org/bluez/hci0");
  attach_interface(data, DBUS_INTERFACE_INTROSPECTABLE);
}

void unregister_dbus_object_path(DBusConnection *conn)
{
  dbus_object_t *dbus_object;
  if(!dbus_connection_get_object_path_data(conn, root.path, (void *)&dbus_object) ||
     dbus_object == NULL)
  {
    return;
  }

  detach_dbus_object(conn, "/org/bluez/hci0");
  detach_dbus_object(conn, "/org/bluez");

  detach_interface(dbus_object, DBUS_INTERFACE_INTROSPECTABLE);
  detach_interface(dbus_object, DBUS_INTERFACE_OBJECT_MANAGER);
  // detach_interface(dbus_object, DBUS_INTERFACE_PROPERTIES);
  detach_dbus_object(conn, root.path);
}

dbus_object_t *attach_dbus_object(DBusConnection *conn, const char *path)
{
  dbus_object_t *dbus_object;
  char *         path_dup       = NULL;
  char *         introspect_dup = NULL;
  if(dbus_connection_get_object_path_data(conn, path, (void *)&dbus_object) ==
     TRUE)
  {
    if(dbus_object != NULL) goto __end;
  }

  dbus_object = malloc(sizeof(dbus_object_t));
  path_dup    = strdup(path);
  introspect_dup =
      strdup(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE "<node></node>");
  if(dbus_object == NULL || path_dup == NULL || introspect_dup == NULL) {
    goto __failed;
  }

  memset(dbus_object, 0, sizeof(dbus_object_t));
  dbus_object->conn       = conn;
  dbus_object->path       = path_dup;
  dbus_object->introspect = introspect_dup;
  interface_sets_init(&dbus_object->interfaces);
  interface_sets_init(&dbus_object->objects);
  interface_sets_init(&dbus_object->added);
  sets_init(&dbus_object->removed, NULL, free);

  if(dbus_connection_register_object_path(conn, path, &server_vtable, dbus_object) ==
     TRUE)
  {
    invalidate_parent_data(conn, path);
    if(attach_interface(dbus_object, DBUS_INTERFACE_INTROSPECTABLE) == TRUE) {
      // todo: notify InterfacesAdded.
      // todo: notify PropertiesChanged on idle.
      goto __end;
    }
  }

__failed:
  free(dbus_object);
  free(path_dup);
  free(introspect_dup);

__end:
  return dbus_object;
}

void detach_dbus_object(DBusConnection *conn, const char *path)
{
  dbus_object_t *dbus_object;
  if(!dbus_connection_get_object_path_data(conn, path, (void *)&dbus_object) ||
     NULL == dbus_object)
  {
    goto __end;
  }

  // todo: notify InterfacesRemoved.
  sets_cleanup(&dbus_object->removed);
  detach_interface(dbus_object, DBUS_INTERFACE_INTROSPECTABLE);
  // todo: notify PropertiesChanged
  invalidate_parent_data(conn, path);

  dbus_connection_unregister_object_path(dbus_object->conn, dbus_object->path);

  free(dbus_object->path);
  free(dbus_object->introspect);
  free(dbus_object);

__end:
  return;
}

static DBusHandlerResult server_message_handler(DBusConnection *conn,
                                                DBusMessage *   message,
                                                void *          data)
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

    result = DBUS_HANDLER_RESULT_HANDLED;
    if(find_interface_method(dbus_object,
                             dbus_message_get_interface(message),
                             dbus_message_get_member(message),
                             &function))
    {
      reply = function(conn, message, data);
    } else {
      dbus_set_error_const(&err, ERROR_INTERFACE ".Failed", "no such method.");
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

static struct dbus_object *invalidate_parent_data(DBusConnection *conn,
                                                  const char *    child_path)
{
  struct dbus_object *data = NULL, *child = NULL, *parent = NULL;
  char *              parent_path, *slash;

  parent_path = strdup(child_path);
  slash       = strrchr(parent_path, '/');
  if(slash == NULL) goto done;

  if(slash == parent_path && parent_path[1] != '\0')
    parent_path[1] = '\0';
  else
    *slash = '\0';

  if(!strlen(parent_path)) goto done;

  if(dbus_connection_get_object_path_data(conn, parent_path, (void *)&data) ==
     FALSE)
  {
    goto done;
  }

  parent = invalidate_parent_data(conn, parent_path);

  if(data == NULL) {
    data = parent;
    if(data == NULL) goto done;
  }

  free(data->introspect);
  data->introspect = NULL;

  if(!dbus_connection_get_object_path_data(conn, child_path, (void *)&child))
    goto done;

  if(child == NULL || sets_find(&data->objects, child, NULL)) goto done;

  sets_add(&data->objects, child);
  child->parent = data;

done:
  free(parent_path);
  return data;
}
