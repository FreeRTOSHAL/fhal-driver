/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <accel.h>
#define ACCEL_PRV
#include <accel_prv.h>


int32_t accel_generic_init(struct accel *accel) {
	struct accel_generic * s = (struct accel_generic *) accel;
	if(hal_isInit(s)) {
		return ACCEL_ALREDY_INITED;
	}
#ifdef CONFIG_ACCEL_THREAD_SAFE
	{
		int32_t ret = hal_init(s);
		if (ret < 0) {
			goto accel_genericInit_error0;
		}
	}
#endif
	s->init = true;
	return 0;
#ifdef CONFIG_ACCEL_THREAD_SAFE
accel_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_ACCEL_MULTI
struct accel *accel_init(uint32_t index);
int32_t accel_deinit(struct accel *accel);

int32_t accel_get(struct accel *accel, struct vector *vector, TickType_t waittime);
int32_t accel_getISR(struct accel *accel, struct vector *vector);
#endif
