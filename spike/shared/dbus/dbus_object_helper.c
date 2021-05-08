/** See a brief introduction (right-hand button) */
#include "dbus_object_helper.h"
/* Private include -----------------------------------------------------------*/
#include "dbus-interface-inner.h"
#include "string-buffer.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int  _interface_find_by_name(const struct interface_data *a,
                                    const struct interface_data *b);
static void _interface_clean_up(struct interface_data *a);

/* Private function ----------------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
void interface_sets_init(sets_t *sets)
{
  sets_init(sets, (CompareCallback_t)_interface_find_by_name, NULL);
}

static int _interface_find_by_name(const struct interface_data *a,
                                   const struct interface_data *b)
{
  return strcmp(a->name, b->name);
}
