/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2019
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <temp.h>
#define TEMP_PRV
#include <temp_prv.h>


int32_t temp_generic_init(struct temp *temp) {
	struct temp_generic * s = (struct temp_generic *) temp;
	if(hal_isInit(s)) {
		return TEMP_ALREDY_INITED;
	}
#ifdef CONFIG_TEMP_THREAD_SAVE
	{
		int32_t ret = hal_init(s);
		if (ret < 0) {
			goto temp_genericInit_error0;
		}
	}
#endif
	s->init = true;
	return 0;
#ifdef CONFIG_TEMP_THREAD_SAVE
temp_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_TEMP_MULTI
struct temp *temp_init(uint32_t index);
int32_t temp_deinit(struct temp *temp);

int32_t temp_get(struct temp *temp, float *value, TickType_t waittime);
int32_t temp_getISR(struct temp *temp, float *value);
#endif
