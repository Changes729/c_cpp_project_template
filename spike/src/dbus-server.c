#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>
#include <unistd.h>

#include "test_dbus_define.h"

typedef struct
{
  enum
  {
    DBUS_NODE_NULL,
    DBUS_NODE_SIGNAL,
    DBUS_NODE_METHOD
  } type;
  const char *name;
  const char *input;
  void (*node_callback)(DBusMessage *msg, DBusConnection *conn);
} dbus_node_t;

static void dbus_main_loop(DBusConnection *dbus_address);
static void reply_to_method_call(DBusMessage *msg, DBusConnection *conn);
static void reply_introspect_method_call(DBusMessage *msg, DBusConnection *conn);
static void send_signal_info(DBusMessage *msg, DBusConnection *conn);

/**
 * dbus-send --session --dest=org.gnome.ServiceName --type=method_call --print-reply=literal --reply-timeout=1000 /org/gnome/ServiceName org.gnome.ServiceName.Method
 */
struct
{
  const char *dbus_name;
  const char *obj;
  const char *interface;
  dbus_node_t nodes[];
} dbus_interfaces = {
    TEST_DBUS_BUS_NAME,
    TEST_DBUS_OBJECT,
    TEST_DBUS_INTERFACE_BASE,
    {
        {DBUS_NODE_METHOD, "Method", "s", reply_to_method_call},
        {DBUS_NODE_METHOD, "Introspect", "s", reply_introspect_method_call},
        {DBUS_NODE_SIGNAL, "Signal", "s", send_signal_info},
        {DBUS_NODE_NULL, NULL, NULL, NULL},
    },
},
  freedesktop_dbus_interface = {
      TEST_DBUS_BUS_NAME,
      "/",
      "org.freedesktop.DBus.Introspectable",
      {
          {DBUS_NODE_METHOD, "Introspect", "s", reply_introspect_method_call},
          {DBUS_NODE_NULL, NULL, NULL, NULL},
      },
};

int main(int agrc, char *argv[])
{
  DBusError err;
  DBusConnection *dbus_address;

  dbus_error_init(&err);

  // dbus address
  dbus_address = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Connection Error(%s) \n", err.message);
    dbus_error_free(&err);
    goto __failed;
  }

  // bus name
  dbus_bus_request_name(dbus_address, TEST_DBUS_BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Requece name error(%s) \n", err.message);
    dbus_error_free(&err);
    goto __failed;
  }

  dbus_main_loop(dbus_address);

__failed:
  return 0;
}

static void dbus_main_loop(DBusConnection *dbus_address)
{
  DBusMessage *msg;

  while (TRUE)
  {
    // non blocking read of the next available message
    dbus_connection_read_write(dbus_address, 0);
    msg = dbus_connection_pop_message(dbus_address);

    if (NULL == msg)
    {
      sleep(1);
      send_signal_info(NULL, dbus_address);
      continue;
    }

    // todo: reply method

    for (dbus_node_t *node = dbus_interfaces.nodes; node->type != DBUS_NODE_NULL; node++)
    {
      if (node->node_callback == NULL)
      {
        // ERROR
        break;
      }
      else if (node->type == DBUS_NODE_METHOD)
      {
        if (dbus_message_is_method_call(msg, dbus_interfaces.interface, node->name))
        {
          node->node_callback(msg, dbus_address);
          break;
        }
      }
    }

    for (dbus_node_t *node = freedesktop_dbus_interface.nodes;
         node->type != DBUS_NODE_NULL; node++)
    {
      if (node->node_callback == NULL)
      {
        // ERROR
        break;
      }
      else if (node->type == DBUS_NODE_METHOD)
      {
        if (dbus_message_is_method_call(msg, freedesktop_dbus_interface.interface, node->name))
        {
          node->node_callback(msg, dbus_address);
          break;
        }
      }
    }

    // todo: reply Properties

    dbus_message_unref(msg);
  }
}

static void reply_to_method_call(DBusMessage *msg, DBusConnection *dbus_address)
{
  DBusMessage *reply = NULL;
  DBusMessageIter iter;
  dbus_bool_t stat = TRUE;
  dbus_uint32_t level = 21614;
  dbus_uint32_t serial = 0;
  char *param = "";

  // read the arguments
  if (!dbus_message_iter_init(msg, &iter))
    fprintf(stderr, "Message has no arguments!\n");
  else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter))
    fprintf(stderr, "Argument is not string!\n");
  else
    dbus_message_iter_get_basic(&iter, &param);

  printf("Method called with %s\n", param);

  reply = dbus_message_new_method_return(msg);
  if (reply == NULL)
  {
    goto end;
  }

  dbus_message_iter_init_append(reply, &iter);
  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_BOOLEAN, &stat))
  {
    fprintf(stderr, "Out Of Memory!\n");
    exit(1);
  }

  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_UINT32, &level))
  {
    fprintf(stderr, "Out Of Memory!\n");
    exit(1);
  }

  if (!dbus_connection_send(dbus_address, reply, &serial))
  {
    fprintf(stderr, "Out Of Memory!\n");
    exit(1);
  }

  dbus_connection_flush(dbus_address);
  dbus_message_unref(reply);
end:
  return;
}

static void reply_introspect_method_call(DBusMessage *msg, DBusConnection *dbus_address)
{
  DBusMessage *reply = NULL;

  reply = dbus_message_new_method_return(msg);
  if (reply == NULL)
  {
    goto end;
  }

  const char *str = TEST_DBUS_SERVER_INTROSPECTION_DESC_XML;
  dbus_message_append_args(reply, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID);

  dbus_connection_send(dbus_address, reply, NULL);
  dbus_message_unref(reply);
end:
  return;
}

static void send_signal_info(DBusMessage *msg, DBusConnection *conn)
{
  dbus_uint32_t serial;
  DBusMessageIter iter;

  msg = dbus_message_new_signal(TEST_DBUS_OBJECT, TEST_DBUS_INTERFACE_BASE, "Signal");
  if (NULL == msg)
  {
    goto end;
  }

  const char *str = "Hello world";

  dbus_message_iter_init_append(msg, &iter);
  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &str))
  {
    fprintf(stderr, "Out of memory\n");
    return;
  }

  if (!dbus_connection_send(conn, msg, &serial))
  {
    fprintf(stderr, "Out of memory");
    return;
  }

  dbus_connection_flush(conn);
  dbus_message_unref(msg);
end:
  return;
}
