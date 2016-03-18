#include <hal.h>
int32_t hal_init(void *data);
int32_t hal_deinit(void *data);
bool hal_isInit(void *data);
int32_t hal_lock(void *data, TickType_t waittime);
int32_t hal_unlock(void *data);
uint32_t *hal_getDev(uint32_t **devs, uint32_t **end, uint32_t index);
