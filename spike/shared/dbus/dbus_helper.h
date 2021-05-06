#ifndef __DBUS_H
#define __DBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dbus/dbus.h>
#include <stdbool.h>
#include <stdint.h>

#include "list.h"

typedef struct DBusArgInfo       DBusArgInfo;
typedef struct DBusMethodTable   DBusMethodTable;
typedef struct DBusSignalTable   DBusSignalTable;
typedef struct DBusPropertyTable DBusPropertyTable;
typedef struct DBusSecurityTable DBusSecurityTable;

typedef void (*DBusWatchFunction)(DBusConnection *connection, void *user_data);

typedef void (*DBusMessageFunction)(DBusConnection *connection,
                                    DBusMessage *   message,
                                    void *          user_data);

typedef bool (*DBusSignalFunction)(DBusConnection *connection,
                                   DBusMessage *   message,
                                   void *          user_data);

DBusConnection *
g_dbus_setup_bus(DBusBusType type, const char *name, DBusError *error);

DBusConnection *
g_dbus_setup_private(DBusBusType type, const char *name, DBusError *error);

bool g_dbus_request_name(DBusConnection *connection,
                         const char *    name,
                         DBusError *     error);

bool g_dbus_set_disconnect_function(DBusConnection *  connection,
                                    DBusWatchFunction function,
                                    void *            user_data,
                                    DBusFreeFunction  destroy);

typedef void (*DBusDestroyFunction)(void *user_data);

typedef DBusMessage *(*DBusMethodFunction)(DBusConnection *connection,
                                           DBusMessage *   message,
                                           void *          user_data);

typedef bool (*DBusPropertyGetter)(const DBusPropertyTable *property,
                                   DBusMessageIter *        iter,
                                   void *                   data);

typedef uint32_t DBusPendingPropertySet;

typedef void (*DBusPropertySetter)(const DBusPropertyTable *property,
                                   DBusMessageIter *        value,
                                   DBusPendingPropertySet   id,
                                   void *                   data);

typedef bool (*DBusPropertyExists)(const DBusPropertyTable *property, void *data);

typedef uint32_t DBusPendingReply;

typedef void (*DBusSecurityFunction)(DBusConnection * connection,
                                     const char *     action,
                                     bool             interaction,
                                     DBusPendingReply pending);

enum DBusFlags {
  G_DBUS_FLAG_ENABLE_EXPERIMENTAL = (1 << 0),
};

enum DBusMethodFlags {
  G_DBUS_METHOD_FLAG_DEPRECATED   = (1 << 0),
  G_DBUS_METHOD_FLAG_NOREPLY      = (1 << 1),
  G_DBUS_METHOD_FLAG_ASYNC        = (1 << 2),
  G_DBUS_METHOD_FLAG_EXPERIMENTAL = (1 << 3),
};

enum DBusSignalFlags {
  G_DBUS_SIGNAL_FLAG_DEPRECATED   = (1 << 0),
  G_DBUS_SIGNAL_FLAG_EXPERIMENTAL = (1 << 1),
};

enum DBusPropertyFlags {
  G_DBUS_PROPERTY_FLAG_DEPRECATED   = (1 << 0),
  G_DBUS_PROPERTY_FLAG_EXPERIMENTAL = (1 << 1),
};

enum DBusSecurityFlags {
  G_DBUS_SECURITY_FLAG_DEPRECATED        = (1 << 0),
  G_DBUS_SECURITY_FLAG_BUILTIN           = (1 << 1),
  G_DBUS_SECURITY_FLAG_ALLOW_INTERACTION = (1 << 2),
};

enum DBusPropertyChangedFlags {
  G_DBUS_PROPERTY_CHANGED_FLAG_FLUSH = (1 << 0),
};

typedef enum DBusMethodFlags          DBusMethodFlags;
typedef enum DBusSignalFlags          DBusSignalFlags;
typedef enum DBusPropertyFlags        DBusPropertyFlags;
typedef enum DBusSecurityFlags        DBusSecurityFlags;
typedef enum DBusPropertyChangedFlags DBusPropertyChangedFlags;

struct DBusArgInfo
{
  const char *name;
  const char *signature;
};

struct DBusMethodTable
{
  const char *       name;
  DBusMethodFunction function;
  DBusMethodFlags    flags;
  unsigned int       privilege;
  const DBusArgInfo *in_args;
  const DBusArgInfo *out_args;
};

struct DBusSignalTable
{
  const char *       name;
  DBusSignalFlags    flags;
  const DBusArgInfo *args;
};

struct DBusPropertyTable
{
  const char *       name;
  const char *       type;
  DBusPropertyGetter get;
  DBusPropertySetter set;
  DBusPropertyExists exists;
  DBusPropertyFlags  flags;
};

struct DBusSecurityTable
{
  unsigned int         privilege;
  const char *         action;
  DBusSecurityFlags    flags;
  DBusSecurityFunction function;
};

#define DBUS_ARGS(args...)                                                     \
  (const DBusArgInfo[])                                                        \
  {                                                                            \
    args, {}                                                                   \
  }

#define DBUS_METHOD(_name, _in_args, _out_args, _function)                     \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function

#define DBUS_ASYNC_METHOD(_name, _in_args, _out_args, _function)               \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_ASYNC

#define DBUS_DEPRECATED_METHOD(_name, _in_args, _out_args, _function)          \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_DEPRECATED

#define DBUS_DEPRECATED_ASYNC_METHOD(_name, _in_args, _out_args, _function)    \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function,                                                       \
  .flags    = G_DBUS_METHOD_FLAG_ASYNC | G_DBUS_METHOD_FLAG_DEPRECATED

#define DBUS_EXPERIMENTAL_METHOD(_name, _in_args, _out_args, _function)        \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_EXPERIMENTAL

#define DBUS_EXPERIMENTAL_ASYNC_METHOD(_name, _in_args, _out_args, _function)  \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function,                                                       \
  .flags    = G_DBUS_METHOD_FLAG_ASYNC | G_DBUS_METHOD_FLAG_EXPERIMENTAL

#define DBUS_NOREPLY_METHOD(_name, _in_args, _out_args, _function)             \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_NOREPLY

#define DBUS_SIGNAL(_name, _args) .name = _name, .args = _args

#define DBUS_DEPRECATED_SIGNAL(_name, _args)                                   \
  .name = _name, .args = _args, .flags = G_DBUS_SIGNAL_FLAG_DEPRECATED

#define DBUS_EXPERIMENTAL_SIGNAL(_name, _args)                                 \
  .name = _name, .args = _args, .flags = G_DBUS_SIGNAL_FLAG_EXPERIMENTAL

void g_dbus_set_flags(int flags);
int  g_dbus_get_flags(void);

bool g_dbus_register_interface(DBusConnection *         connection,
                               const char *             path,
                               const char *             name,
                               const DBusMethodTable *  methods,
                               const DBusSignalTable *  signals,
                               const DBusPropertyTable *properties,
                               void *                   user_data,
                               DBusDestroyFunction      destroy);
bool g_dbus_unregister_interface(DBusConnection *connection,
                                 const char *    path,
                                 const char *    name);

bool g_dbus_register_security(const DBusSecurityTable *security);
bool g_dbus_unregister_security(const DBusSecurityTable *security);

void g_dbus_pending_success(DBusConnection *connection, DBusPendingReply pending);
void g_dbus_pending_error(DBusConnection * connection,
                          DBusPendingReply pending,
                          const char *     name,
                          const char *     format,
                          ...) __attribute__((format(printf, 4, 5)));
void g_dbus_pending_error_valist(DBusConnection * connection,
                                 DBusPendingReply pending,
                                 const char *     name,
                                 const char *     format,
                                 va_list          args);

DBusMessage *
g_dbus_create_error(DBusMessage *message, const char *name, const char *format, ...)
    __attribute__((format(printf, 3, 4)));
DBusMessage *g_dbus_create_error_valist(DBusMessage *message,
                                        const char * name,
                                        const char * format,
                                        va_list      args);
DBusMessage *g_dbus_create_reply(DBusMessage *message, int type, ...);
DBusMessage *
g_dbus_create_reply_valist(DBusMessage *message, int type, va_list args);

bool g_dbus_send_message(DBusConnection *connection, DBusMessage *message);
bool g_dbus_send_message_with_reply(DBusConnection *  connection,
                                    DBusMessage *     message,
                                    DBusPendingCall **call,
                                    int               timeout);
bool g_dbus_send_error(DBusConnection *connection,
                       DBusMessage *   message,
                       const char *    name,
                       const char *    format,
                       ...) __attribute__((format(printf, 4, 5)));
bool g_dbus_send_error_valist(DBusConnection *connection,
                              DBusMessage *   message,
                              const char *    name,
                              const char *    format,
                              va_list         args);
bool g_dbus_send_reply(DBusConnection *connection,
                       DBusMessage *   message,
                       int             type,
                       ...);
bool g_dbus_send_reply_valist(DBusConnection *connection,
                              DBusMessage *   message,
                              int             type,
                              va_list         args);

bool g_dbus_emit_signal(DBusConnection *connection,
                        const char *    path,
                        const char *    interface,
                        const char *    name,
                        int             type,
                        ...);
bool g_dbus_emit_signal_valist(DBusConnection *connection,
                               const char *    path,
                               const char *    interface,
                               const char *    name,
                               int             type,
                               va_list         args);

unsigned g_dbus_add_service_watch(DBusConnection *    connection,
                                  const char *        name,
                                  DBusWatchFunction   connect,
                                  DBusWatchFunction   disconnect,
                                  void *              user_data,
                                  DBusDestroyFunction destroy);
unsigned g_dbus_add_disconnect_watch(DBusConnection *    connection,
                                     const char *        name,
                                     DBusWatchFunction   function,
                                     void *              user_data,
                                     DBusDestroyFunction destroy);
unsigned g_dbus_add_signal_watch(DBusConnection *    connection,
                                 const char *        sender,
                                 const char *        path,
                                 const char *        interface,
                                 const char *        member,
                                 DBusSignalFunction  function,
                                 void *              user_data,
                                 DBusDestroyFunction destroy);
unsigned g_dbus_add_properties_watch(DBusConnection *    connection,
                                     const char *        sender,
                                     const char *        path,
                                     const char *        interface,
                                     DBusSignalFunction  function,
                                     void *              user_data,
                                     DBusDestroyFunction destroy);
bool     g_dbus_remove_watch(DBusConnection *connection, unsigned tag);
void     g_dbus_remove_all_watches(DBusConnection *connection);

void g_dbus_pending_property_success(DBusPendingPropertySet id);
void g_dbus_pending_property_error_valist(DBusPendingReply id,
                                          const char *     name,
                                          const char *     format,
                                          va_list          args);
void g_dbus_pending_property_error(DBusPendingReply id,
                                   const char *     name,
                                   const char *     format,
                                   ...);

/*
 * Note that when multiple properties for a given object path are changed
 * in the same mainloop iteration, they will be grouped with the last
 * property changed. If this behaviour is undesired, use
 * g_dbus_emit_property_changed_full() with the
 * G_DBUS_PROPERTY_CHANGED_FLAG_FLUSH flag, causing the signal to ignore
 * any grouping.
 */
void g_dbus_emit_property_changed(DBusConnection *connection,
                                  const char *    path,
                                  const char *    interface,
                                  const char *    name);
void g_dbus_emit_property_changed_full(DBusConnection *         connection,
                                       const char *             path,
                                       const char *             interface,
                                       const char *             name,
                                       DBusPropertyChangedFlags flags);
bool g_dbus_get_properties(DBusConnection * connection,
                           const char *     path,
                           const char *     interface,
                           DBusMessageIter *iter);

bool g_dbus_attach_object_manager(DBusConnection *connection);
bool g_dbus_detach_object_manager(DBusConnection *connection);

typedef struct DBusClient DBusClient;
typedef struct DBusProxy  DBusProxy;

DBusProxy *
g_dbus_proxy_new(DBusClient *client, const char *path, const char *interface);

DBusProxy *g_dbus_proxy_ref(DBusProxy *proxy);
void       g_dbus_proxy_unref(DBusProxy *proxy);

const char *g_dbus_proxy_get_path(const DBusProxy *proxy);
const char *g_dbus_proxy_get_interface(DBusProxy *proxy);

bool g_dbus_proxy_get_property(DBusProxy *      proxy,
                               const char *     name,
                               DBusMessageIter *iter);

DBusProxy *g_dbus_proxy_lookup(struct list_head *list,
                               int *             index,
                               const char *      path,
                               const char *      interface);
char *
g_dbus_proxy_path_lookup(struct list_head *list, int *index, const char *path);

bool g_dbus_proxy_refresh_property(DBusProxy *proxy, const char *name);

typedef void (*DBusResultFunction)(const DBusError *error, void *user_data);

bool g_dbus_proxy_set_property_basic(DBusProxy *         proxy,
                                     const char *        name,
                                     int                 type,
                                     const void *        value,
                                     DBusResultFunction  function,
                                     void *              user_data,
                                     DBusDestroyFunction destroy);

bool g_dbus_proxy_set_property_array(DBusProxy *         proxy,
                                     const char *        name,
                                     int                 type,
                                     const void *        value,
                                     size_t              size,
                                     DBusResultFunction  function,
                                     void *              user_data,
                                     DBusDestroyFunction destroy);

void g_dbus_dict_append_entry(DBusMessageIter *dict,
                              const char *     key,
                              int              type,
                              void *           val);
void g_dbus_dict_append_basic_array(DBusMessageIter *dict,
                                    int              key_type,
                                    const void *     key,
                                    int              type,
                                    void *           val,
                                    int              n_elements);
void g_dbus_dict_append_array(
    DBusMessageIter *dict, const char *key, int type, void *val, int n_elements);

typedef void (*DBusSetupFunction)(DBusMessageIter *iter, void *user_data);
typedef void (*DBusReturnFunction)(DBusMessage *message, void *user_data);

bool g_dbus_proxy_method_call(DBusProxy *         proxy,
                              const char *        method,
                              DBusSetupFunction   setup,
                              DBusReturnFunction  function,
                              void *              user_data,
                              DBusDestroyFunction destroy);

typedef void (*DBusClientFunction)(DBusClient *client, void *user_data);
typedef void (*DBusProxyFunction)(DBusProxy *proxy, void *user_data);
typedef void (*DBusPropertyFunction)(DBusProxy *      proxy,
                                     const char *     name,
                                     DBusMessageIter *iter,
                                     void *           user_data);

bool g_dbus_proxy_set_property_watch(DBusProxy *          proxy,
                                     DBusPropertyFunction function,
                                     void *               user_data);

bool g_dbus_proxy_set_removed_watch(DBusProxy *       proxy,
                                    DBusProxyFunction destroy,
                                    void *            user_data);

DBusClient *g_dbus_client_new(DBusConnection *connection,
                              const char *    service,
                              const char *    path);
DBusClient *g_dbus_client_new_full(DBusConnection *connection,
                                   const char *    service,
                                   const char *    path,
                                   const char *    root_path);

DBusClient *g_dbus_client_ref(DBusClient *client);
void        g_dbus_client_unref(DBusClient *client);

bool g_dbus_client_set_connect_watch(DBusClient *      client,
                                     DBusWatchFunction function,
                                     void *            user_data);
bool g_dbus_client_set_disconnect_watch(DBusClient *      client,
                                        DBusWatchFunction function,
                                        void *            user_data);
bool g_dbus_client_set_signal_watch(DBusClient *        client,
                                    DBusMessageFunction function,
                                    void *              user_data);
bool g_dbus_client_set_ready_watch(DBusClient *       client,
                                   DBusClientFunction ready,
                                   void *             user_data);
bool g_dbus_client_set_proxy_handlers(DBusClient *         client,
                                      DBusProxyFunction    proxy_added,
                                      DBusProxyFunction    proxy_removed,
                                      DBusPropertyFunction property_changed,
                                      void *               user_data);

#ifdef __cplusplus
}
#endif

#endif /* __DBUS_H */
