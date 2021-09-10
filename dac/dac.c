/* SPDX-License-Identifier: MIT */
/*
 * Author: David Brandt <david@wakka.de>
 * Date: 2021
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <dac.h>
#define DAC_PRV
#include <dac_prv.h>
int32_t dac_generic_init(struct dac *a) {
	struct dac_generic *dac = (struct dac_generic *) a;
	if(dac->init) {
		return DAC_ALREDY_INITED;
	}
#ifdef CONFIG_DAC_THREAD_SAFE
	{
		int ret = hal_init(dac);
		if (ret < 0) {
			goto dac_genericInit_error0;
		}
	}
#endif
	dac->init = true;
	return 0;
#ifdef CONFIG_DAC_THREAD_SAFE
dac_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_DAC_MULTI
struct dac *dac_init(uint32_t index, uint8_t bits);
int32_t dac_deinit(struct dac *dac);
int32_t dac_set(struct dac *dac, uint32_t data, TickType_t waittime);
int32_t dac_setISR(struct dac *dac, uint32_t data);
#endif
