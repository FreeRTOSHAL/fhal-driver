#include <hal.h>
extern uint32_t _dev_hal;
void **hals = (void **) &_dev_hal;

int32_t hal_init(void *data);
int32_t hal_deinit(void *data);
bool hal_isInit(void *data);
int32_t hal_lock(void *data, TickType_t waittime);
int32_t hal_unlock(void *data);
