#ifndef TEST_DBUS_H
#define TEST_DBUS_H

#define TEST_DBUS_BUS_NAME "org.gnome.ServiceName"
#define TEST_DBUS_PATH_BASE "/org/gnome/ServiceName"
#define TEST_DBUS_INTERFACE_BASE "org.gnome.ServiceName"
#define TEST_DBUS_NODE TEST_DBUS_BUS_NAME
#define TEST_DBUS_OBJECT TEST_DBUS_PATH_BASE

#define TEST_DBUS_SERVER_INTROSPECTION_DESC_XML                                \
  DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE                                    \
  "<node>\n"                                                                   \
  " <interface name='org.freedesktop.DBus.Introspectable'>\n"                  \
  "   <method name='Introspect'>\n"                                            \
  "     <arg name='xml' type='s' direction='out' />\n"                         \
  "   </method>\n"                                                             \
  " </interface>\n"                                                            \
  "</node>\n"

#endif //! TEST_DBUS_H