/** See a brief introduction (right-hand button) */
#include "dbus_object_helper.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define G_DBUS_ANNOTATE(name_, value_)                                         \
  "<annotation name=\"org.freedesktop.DBus." name_ "\"value=\"" value_ "\"/>"

#define G_DBUS_ANNOTATE_DEPRECATED G_DBUS_ANNOTATE("Deprecated", "true")
#define G_DBUS_ANNOTATE_NOREPLY    G_DBUS_ANNOTATE("Method.NoReply", "true")

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static struct string *string_new(const char *str);
static int string_append_printf(struct string *, const char *format, ...);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
static void print_arguments(struct string *     gstr,
                            const DBusArgInfo *args,
                            const char *        direction)
{
  for(; args && args->name; args++) {
    string_append_printf(gstr,
                         "<arg name=\"%s\" type=\"%s\"",
                         args->name,
                         args->signature);

    if(direction)
      string_append_printf(gstr, " direction=\"%s\"/>\n", direction);
    else
      string_append_printf(gstr, "/>\n");
  }
}

void generate_interface_xml(struct string *gstr, struct interface_data *iface)
{
  const DBusMethodTable *   method;
  const DBusSignalTable *  signal;
  const DBusPropertyTable *property;

  for(method = iface->methods; method && method->name; method++) {
    // if(check_experimental(method->flags, G_DBUS_METHOD_FLAG_EXPERIMENTAL))
    //   continue;

    string_append_printf(gstr, "<method name=\"%s\">", method->name);
    print_arguments(gstr, method->in_args, "in");
    print_arguments(gstr, method->out_args, "out");

    if(method->flags & G_DBUS_METHOD_FLAG_DEPRECATED)
      string_append_printf(gstr, G_DBUS_ANNOTATE_DEPRECATED);

    if(method->flags & G_DBUS_METHOD_FLAG_NOREPLY)
      string_append_printf(gstr, G_DBUS_ANNOTATE_NOREPLY);

    string_append_printf(gstr, "</method>");
  }

  for(signal = iface->signals; signal && signal->name; signal++) {
    // if(check_experimental(signal->flags, G_DBUS_SIGNAL_FLAG_EXPERIMENTAL))
    //   continue;

    string_append_printf(gstr, "<signal name=\"%s\">", signal->name);
    print_arguments(gstr, signal->args, NULL);

    if(signal->flags & G_DBUS_SIGNAL_FLAG_DEPRECATED)
      string_append_printf(gstr, G_DBUS_ANNOTATE_DEPRECATED);

    string_append_printf(gstr, "</signal>\n");
  }

  for(property = iface->properties; property && property->name; property++) {
    // if(check_experimental(property->flags, G_DBUS_PROPERTY_FLAG_EXPERIMENTAL))
    //   continue;

    string_append_printf(gstr,
                         "<property name=\"%s\""
                         " type=\"%s\" access=\"%s%s\">",
                         property->name,
                         property->type,
                         property->get ? "read" : "",
                         property->set ? "write" : "");

    if(property->flags & G_DBUS_PROPERTY_FLAG_DEPRECATED)
      string_append_printf(gstr, G_DBUS_ANNOTATE_DEPRECATED);

    string_append_printf(gstr, "</property>");
  }
}

void generate_introspection_xml(DBusConnection *     conn,
                                struct generic_data *data,
                                const char *         path)
{
  struct list *  list;
  struct string *gstr;
  char **        children __attribute__((unused));

  free(data->introspect);

  gstr = string_new(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE);

  string_append_printf(gstr, "<node>");

  list_for_each_entry(list, &data->interfaces.head, head)
  {
    struct interface_data *iface = list->data;
    string_append_printf(gstr, "<interface name=\"%s\">", iface->name);

    generate_interface_xml(gstr, iface);

    string_append_printf(gstr, "</interface>");
  }

#if 0
  if(!dbus_connection_list_registered(conn, path, &children)) goto done;

  for(size_t i = 0; children[i]; i++)
    string_append_printf(gstr, "<node name=\"%s\"/>", children[i]);

  dbus_free_string_array(children);
#endif

done:
  string_append_printf(gstr, "</node>");

  data->introspect = gstr->buffer;
  free(gstr);
}

static struct string *string_new(const char *str)
{
  struct string *string = malloc(sizeof(struct string));

  if(NULL != str) {
    string->length      = strlen(str);
    string->buffer_size = (string->length << 1);
  } else  // str == NULL
  {
    string->length      = 0;
    string->buffer_size = 16;
    str                 = "";
  }

  string->buffer = malloc(string->buffer_size);
  strcpy(string->buffer, str);

  return string;
}

static int string_append_printf(struct string *string, const char *format, ...)
{
  va_list args, copy;
  va_start(args, format);
  va_copy(copy, args);

  size_t length = vsnprintf(NULL, 0, format, args) + 1;
  char   buffer[length];
  vsnprintf(buffer, length, format, copy);

  va_end(copy);
  va_end(args);

  if(string->length + length >= string->buffer_size) {
    if(string->buffer_size < (SIZE_MAX >> 1)) {
      string->buffer_size <<= 1;
    } else {
      string->buffer_size = SIZE_MAX;
    }

    char *tmpbuf = realloc(string->buffer, string->buffer_size);
    if(tmpbuf != NULL) {
      string->buffer = tmpbuf;
    } else {
      length = -1;
      goto __failed;
    }
  }

  length = snprintf(&string->buffer[string->length],
                    string->buffer_size - string->length,
                    "%s",
                    buffer);
  string->length += length;

__failed:
  return length;
}