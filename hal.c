/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#include <hal.h>
#include <stdio.h>
#include <inttypes.h>
int32_t hal_init(void *data);
int32_t hal_deinit(void *data);
bool hal_isInit(void *data);
int32_t hal_lock(void *data, TickType_t waittime);
int32_t hal_unlock(void *data);
uintptr_t *hal_getDev(uintptr_t **devs, uintptr_t **end, uint32_t index);
#ifdef CONFIG_INSTANCE_NAME
int32_t hal_printNames() {
	HAL_DEFINE_GLOBAL_ARRAY(hal);
	uint32_t i = 0;
	uintptr_t **instances;
	struct hal *hal;
	if (&_devs != &_devs_end) {
		for(instances = &_devs; instances < &_devs_end;instances++, i++) {
			hal = (struct hal *) *instances;
			printf("%" PRId32 ": %s\n", i, hal->name);
		}
	}
	return 0;
}
#endif
