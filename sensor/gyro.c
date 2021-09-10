/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <gyro.h>
#define GYRO_PRV
#include <gyro_prv.h>


int32_t gyro_generic_init(struct gyro *gyro) {
	struct gyro_generic * s = (struct gyro_generic *) gyro;
	if(hal_isInit(s)) {
		return GYRO_ALREDY_INITED;
	}
#ifdef CONFIG_GYRO_THREAD_SAFE
	{
		int32_t ret = hal_init(s);
		if (ret < 0) {
			goto gyro_genericInit_error0;
		}
	}
#endif
	s->init = true;
	return 0;
#ifdef CONFIG_GYRO_THREAD_SAFE
gyro_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_GYRO_MULTI
struct gyro *gyro_init(uint32_t index);
int32_t gyro_deinit(struct gyro *gyro);

int32_t gyro_get(struct gyro *gyro, struct vector *vector, TickType_t waittime);
int32_t gyro_getISR(struct gyro *gyro, struct vector *vector);
#endif
