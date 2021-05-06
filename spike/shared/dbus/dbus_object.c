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
#include "timer-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DBusHandlerResult  server_message_handler(DBusConnection *,
                                                 DBusMessage *,
                                                 void *user_data);
static DBusMethodFunction find_interface_method(DBusMessage *,
                                                struct dbus_object *);

static DBusMessage *introspect(DBusConnection *, DBusMessage *, void *user_data);

static int          append_object(void *data, void *user_data);
static DBusMessage *get_objects(DBusConnection *connection,
                                DBusMessage *   message,
                                void *          user_data);
static void append_interfaces(struct dbus_object *data, DBusMessageIter *iter);
static int  append_interface(void *data, void *user_data);

static struct dbus_object *invalidate_parent_data(DBusConnection *conn,
                                                  const char *    child_path);
static void                emit_interfaces_added(struct dbus_object *data);

static void event_idle(struct dbus_object *data);
static void process_changes(void *user_data);

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
                 get_objects)},
    {},
};

static const DBusSignalTable object_signals[] = {
    {DBUS_SIGNAL("InterfacesAdded",
                 DBUS_ARGS({"object", "o"}, {"interfaces", "a{sa{sv}}"}))},
    {DBUS_SIGNAL("InterfacesRemoved",
                 DBUS_ARGS({"object", "o"}, {"interfaces", "as"}))},
    {},
};

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

  add_interface(data,
                DBUS_INTERFACE_INTROSPECTABLE,
                introspect_methods,
                NULL,
                NULL,
                data,
                NULL);
  add_interface(data,
                DBUS_INTERFACE_PROPERTIES,
                properties_methods,
                properties_signals,
                NULL,
                data,
                NULL);
  add_interface(data,
                DBUS_INTERFACE_OBJECT_MANAGER,
                object_methods,
                object_signals,
                NULL,
                data,
                NULL);

  data = attach_dbus_object(conn, "/org/bluez");
  add_interface(data,
                DBUS_INTERFACE_INTROSPECTABLE,
                introspect_methods,
                NULL,
                NULL,
                data,
                NULL);

  data = attach_dbus_object(conn, "/org/bluez/hci0");
  add_interface(data,
                DBUS_INTERFACE_INTROSPECTABLE,
                introspect_methods,
                NULL,
                NULL,
                data,
                NULL);
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
  detach_dbus_object(conn, "/org/bluez/");

  remove_interface(dbus_object, DBUS_INTERFACE_INTROSPECTABLE);
  remove_interface(dbus_object, DBUS_INTERFACE_PROPERTIES);
  remove_interface(dbus_object, DBUS_INTERFACE_OBJECT_MANAGER);
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
    if(add_interface(dbus_object,
                     DBUS_INTERFACE_INTROSPECTABLE,
                     introspect_methods,
                     NULL,
                     NULL,
                     conn,
                     NULL) == TRUE)
    {
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
  remove_interface(dbus_object, DBUS_INTERFACE_INTROSPECTABLE);
  // todo: notify PropertiesChanged
  invalidate_parent_data(conn, path);

  dbus_connection_unregister_object_path(dbus_object->conn, dbus_object->path);

  free(dbus_object->path);
  free(dbus_object->introspect);
  free(dbus_object);

__end:
  return;
}

bool add_interface(dbus_object_t *          data,
                   const char *             name,
                   const DBusMethodTable *  methods,
                   const DBusSignalTable *  signals,
                   const DBusPropertyTable *properties,
                   void *                   user_data,
                   DBusDestroyFunction      destroy)
{
  interface_data_t *iface    = NULL;
  char *            name_dup = NULL;
  if(find_interface_by_name(data, name, &iface) == true) {
    goto __end;
  }

  name_dup = strdup(name);
  iface    = malloc(sizeof(interface_data_t));
  if(name_dup == NULL || iface == NULL) {
    goto __failed;
  }

  memset(iface, 0, sizeof(interface_data_t));
  iface->name       = name_dup;
  iface->methods    = methods;
  iface->signals    = signals;
  iface->properties = properties;
  iface->user_data  = user_data;
  iface->destroy    = destroy;

  sets_add(&data->interfaces, iface);
  if(sets_find(&data->interfaces, iface, NULL) == true) {
    free(data->introspect);
    data->introspect = NULL;
    sets_add(&data->added, iface);
    event_idle(data);
    // todo: notify interfaceAdd.
    goto __end;
  }

__failed:
  free(name_dup);
  free(iface);

__end:
  return iface != NULL;
}

void remove_interface(dbus_object_t *data, const char *name)
{
  interface_data_t *iface = NULL;
  if(find_interface_by_name(data, name, &iface) == false) {
    goto __end;
  }

  // todo: process_properties_from_interface(data, iface);
  sets_remove(&data->interfaces, iface);

  if(iface->destroy) {
    iface->destroy(iface->user_data);
    iface->user_data = NULL;
  }

  if(sets_find(&data->added, iface, NULL)) {
    sets_remove(&data->added, iface);
    goto __clean;
  }

  if(data->parent == NULL) {
    goto __clean;
  }

  sets_add(&data->removed, iface->name);
  iface->name = NULL;

__clean:
  free(iface->name);
  free(iface);

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

static DBusMethodFunction find_interface_method(DBusMessage *       message,
                                                struct dbus_object *dbus_object)
{
  const char *interface_name = dbus_message_get_interface(message);
  struct interface_find_method_by_name_pkg pkg = {interface_name, message, NULL};

  sets_foreach(&dbus_object->interfaces, interface_find_method_by_name, &pkg);

  return pkg.method;
}

static DBusMessage *introspect(DBusConnection *connection,
                               DBusMessage *   message,
                               void *          user_data)
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

static bool check_experimental(int flags, int flag)
{
  static int global_flags = 0;
  if(!(flags & flag)) return FALSE;

  return !(global_flags & G_DBUS_FLAG_ENABLE_EXPERIMENTAL);
}

static void append_property(struct interface_data *  iface,
                            const DBusPropertyTable *p,
                            DBusMessageIter *        dict)
{
  DBusMessageIter entry, value;

  dbus_message_iter_open_container(dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &p->name);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, p->type, &value);

  p->get(p, &value, iface->user_data);

  dbus_message_iter_close_container(&entry, &value);
  dbus_message_iter_close_container(dict, &entry);
}

static void append_properties(struct interface_data *data, DBusMessageIter *iter)
{
  DBusMessageIter          dict;
  const DBusPropertyTable *p;

  // clang-format off
  dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
          DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
          DBUS_TYPE_STRING_AS_STRING
          DBUS_TYPE_VARIANT_AS_STRING
          DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
          &dict);
  // clang-format on

  for(p = data->properties; p && p->name; p++) {
    if(check_experimental(p->flags, G_DBUS_PROPERTY_FLAG_EXPERIMENTAL))
      continue;

    if(p->get == NULL) continue;

    if(p->exists != NULL && !p->exists(p, data->user_data)) continue;

    append_property(data, p, &dict);
  }

  dbus_message_iter_close_container(iter, &dict);
}

static int append_interface(void *data, void *user_data)
{
  struct interface_data *iface = data;
  DBusMessageIter *      array = user_data;
  DBusMessageIter        entry;

  dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &iface->name);
  append_properties(data, &entry);
  dbus_message_iter_close_container(array, &entry);

  return true;
}

static void append_interfaces(struct dbus_object *data, DBusMessageIter *iter)
{
  DBusMessageIter array;

  // clang-format off
  dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
          DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
          DBUS_TYPE_STRING_AS_STRING
          DBUS_TYPE_ARRAY_AS_STRING
          DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
          DBUS_TYPE_STRING_AS_STRING
          DBUS_TYPE_VARIANT_AS_STRING
          DBUS_DICT_ENTRY_END_CHAR_AS_STRING
          DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
          &array);
  // clang-format on

  sets_foreach(&data->interfaces, append_interface, &array);

  dbus_message_iter_close_container(iter, &array);
}

static int append_object(void *data, void *user_data)
{
  struct dbus_object *child = data;
  DBusMessageIter *   array = user_data;
  DBusMessageIter     entry;

  dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_OBJECT_PATH, &child->path);
  append_interfaces(child, &entry);
  dbus_message_iter_close_container(array, &entry);

  sets_foreach(&child->objects, append_object, user_data);

  return true;
}

static DBusMessage *get_objects(DBusConnection *connection,
                                DBusMessage *   message,
                                void *          user_data)
{
  struct dbus_object *data = user_data;
  DBusMessage *       reply;
  DBusMessageIter     iter;
  DBusMessageIter     array;

  reply = dbus_message_new_method_return(message);
  if(reply == NULL) return NULL;

  dbus_message_iter_init_append(reply, &iter);

  // clang-format off
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_OBJECT_PATH_AS_STRING
					DBUS_TYPE_ARRAY_AS_STRING
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_STRING_AS_STRING
					DBUS_TYPE_ARRAY_AS_STRING
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_STRING_AS_STRING
					DBUS_TYPE_VARIANT_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
					&array);
  // clang-format on

  sets_foreach(&data->objects, append_object, &array);

  dbus_message_iter_close_container(&iter, &array);

  return reply;
}

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

static int append_name(void *data, void *user_data)
{
  char *           name = data;
  DBusMessageIter *iter = user_data;

  dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &name);

  return true;
}

static void emit_interfaces_added(struct dbus_object *data)
{
  DBusMessage *   signal;
  DBusMessageIter iter, array;

  if(strcmp(data->path, root.path) == 0) return;

  signal = dbus_message_new_signal(root.path,
                                   DBUS_INTERFACE_OBJECT_MANAGER,
                                   "InterfacesAdded");
  if(signal == NULL) return;

  dbus_message_iter_init_append(signal, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &data->path);

  // clang-format off
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_ARRAY_AS_STRING
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &array);
  // clang-format on

  sets_foreach(&data->added, append_interface, &array);
  sets_cleanup(&data->added);

  dbus_message_iter_close_container(&iter, &array);

  /* Use dbus_connection_send to avoid recursive calls to g_dbus_flush */
  dbus_connection_send(data->conn, signal, NULL);
  dbus_message_unref(signal);
}

static void emit_interfaces_removed(struct dbus_object *data)
{
  DBusMessage *   signal;
  DBusMessageIter iter, array;

  if(strcmp(data->path, root.path) == 0) return;

  signal = dbus_message_new_signal(root.path,
                                   DBUS_INTERFACE_OBJECT_MANAGER,
                                   "InterfacesRemoved");
  if(signal == NULL) return;

  dbus_message_iter_init_append(signal, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &data->path);
  dbus_message_iter_open_container(&iter,
                                   DBUS_TYPE_ARRAY,
                                   DBUS_TYPE_STRING_AS_STRING,
                                   &array);

  sets_foreach(&data->removed, append_name, &array);
  sets_cleanup(&data->removed);

  dbus_message_iter_close_container(&iter, &array);

  /* Use dbus_connection_send to avoid recursive calls to g_dbus_flush */
  dbus_connection_send(data->conn, signal, NULL);
  dbus_message_unref(signal);
}

static void event_idle(struct dbus_object *data)
{
  timer_task_new(0, process_changes, data);
}

static void process_changes(void *user_data)
{
  struct dbus_object *data = user_data;

  // remove_pending(data);

  if(sets_length(&data->added) > 0) emit_interfaces_added(data);

  /* Flush pending properties */
  // if(data->pending_prop == TRUE) process_property_changes(data);

  if(sets_length(&data->removed) > 0) emit_interfaces_removed(data);

  // data->process_id = 0;
}
