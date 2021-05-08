/** See a brief introduction (right-hand button) */
#include "dbus-interface-home.h"
/* Private include -----------------------------------------------------------*/
#include <string.h>

#include "dbus-interface-inner.h"
#include "dbus_define.h"
#include "dbus_object_helper.h"
#include "introspectable/introspectable.h"
#include "object_manager/object_manager.h"
#include "properties/properties.h"
#include "set-list.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef interface_data_t* (*register_iface_cb)(dbus_object_t* data);
typedef void (*unregister_iface_cb)(dbus_object_t* data);

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct iface_register_desc
{
  const char*         iface_name;
  register_iface_cb   regist_callback;
  unregister_iface_cb unregist_callback;
} iface_registers_map[] = {
    {DBUS_INTERFACE_INTROSPECTABLE, introspectable_regist, introspectable_unregist},
    {DBUS_INTERFACE_OBJECT_MANAGER, object_manager_regist, object_manager_unregist},
    {DBUS_INTERFACE_PROPERTIES, properties_regist, properties_unregist},
    {NULL, NULL},
};

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static register_iface_cb   _find_regist(const char* name);
static unregister_iface_cb _find_unregist(const char* name);

/* Private function ----------------------------------------------------------*/
bool attach_interface(struct dbus_object* data, const char* interface_name)
{
  return _find_regist(interface_name)(data) != NULL;
}

void detach_interface(struct dbus_object* data, const char* interface_name)
{
  _find_unregist(interface_name)(data);
}

bool find_interface_method(struct dbus_object* dbus_object,
                           const char*         iface_name,
                           const char*         method_name,
                           DBusMethodFunction* method)
{
  list_head_t* list_head = sets_get_listhead(&dbus_object->interfaces);
  bool         find      = false;

  struct interface_data* interface;
  list_foreach_data(interface, list_head)
  {
    if(strcmp(interface->name, iface_name) != 0) {
      continue;
    }

    for(const DBusMethodTable* node = interface->methods; NULL != node->name;
        node++)
    {
      if(strcmp(node->name, method_name) == 0) {
        find = true;
        if(method != NULL) {
          *method = node->function;
        }
        break;
      }
    }
  }

  return find;
}

static register_iface_cb _find_regist(const char* name)
{
  for(size_t i = 0; iface_registers_map[i].iface_name != NULL; i++) {
    if(strcmp(iface_registers_map[i].iface_name, name) == 0) {
      return iface_registers_map[i].regist_callback;
    }
  }

  return NULL;
}

static unregister_iface_cb _find_unregist(const char* name)
{
  for(size_t i = 0; iface_registers_map[i].iface_name != NULL; i++) {
    if(strcmp(iface_registers_map[i].iface_name, name) == 0) {
      return iface_registers_map[i].unregist_callback;
    }
  }

  return NULL;
}
