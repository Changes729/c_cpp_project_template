/** See a brief introduction (right-hand button) */
#include "dbus_object_helper.h"
/* Private include -----------------------------------------------------------*/
#include "string-buffer.h"

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
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
static void print_arguments(struct string *    gstr,
                            const DBusArgInfo *args,
                            const char *       direction)
{
  for(; args && args->name; args++) {
    string_append_printf(gstr,
                         "<arg name=\"%s\" type=\"%s\"",
                         args->name,
                         args->signature);

    if(direction)
      string_append_printf(gstr, " direction=\"%s\"/>", direction);
    else
      string_append_printf(gstr, "/>");
  }
}

void generate_interface_xml(struct string *gstr, struct interface_data *iface)
{
  const DBusMethodTable *  method;
  const DBusSignalTable *  signal;
  const DBusPropertyTable *property;

  for(method = iface->methods; method && method->name; method++) {
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
    string_append_printf(gstr, "<signal name=\"%s\">", signal->name);
    print_arguments(gstr, signal->args, NULL);

    if(signal->flags & G_DBUS_SIGNAL_FLAG_DEPRECATED)
      string_append_printf(gstr, G_DBUS_ANNOTATE_DEPRECATED);

    string_append_printf(gstr, "</signal>");
  }

  for(property = iface->properties; property && property->name; property++) {
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

  list_foreach(list, sets_get_listhead(&data->interfaces))
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

  data->introspect = strdup(string_c_str(gstr));
  string_delete(gstr);
}
