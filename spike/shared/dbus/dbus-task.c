/** See a brief introduction (right-hand button) */
#include "dbus-task.h"
/* Private include -----------------------------------------------------------*/
#include "object-manager/object-manager.h"
#include "properties/properties.h"
#include "set-list.h"
#include "timer-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static timer_task_t *_queue_dispatch_task = NULL;
sets_t               _queue_object_update_tasks;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void message_dispatch(void *data);
void        process_pading(void *user_data);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
void queue_dispatch(DBusConnection *connection)
{
  if(dbus_connection_get_dispatch_status(connection) == DBUS_DISPATCH_DATA_REMAINS)
    if(_queue_dispatch_task == NULL)
      _queue_dispatch_task = timer_task_new(0, message_dispatch, connection);
}

void queue_pading(dbus_object_t *data)
{
  if(!sets_find(&_queue_object_update_tasks, data, NULL)) {
    sets_add(&_queue_object_update_tasks, data);
    // ! task should run before dbus_object destroy.
    timer_task_new(0, process_pading, data);
  }
}

static void message_dispatch(void *data)
{
  DBusConnection *connection = data;
  while(dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS) {
  }

  _queue_dispatch_task = NULL;
}

void process_pading(void *user_data)
{
  struct dbus_object *data = user_data;

  if(sets_length(&data->added) > 0) emit_interfaces_added(data);

  /* Flush pending properties */
  if(data->pending_prop == TRUE) process_property_changes(data);

  if(sets_length(&data->removed) > 0) emit_interfaces_removed(data);

  sets_remove(&_queue_object_update_tasks, data);
}