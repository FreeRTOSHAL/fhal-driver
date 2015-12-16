#include <hal.h>
int32_t hal_init(void *data);
bool hal_isInit(void *data);
int32_t hal_lock(void *data, TickType_t waittime);
int32_t hal_unlock(void *data);
