/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2019
 */
#ifndef TEMP_NTC_H_
#define TEMP_NTC_H_
/**\cond INTERNAL*/
#include <temp.h>
#define TEMP_PRV
#include <temp_prv.h>
#include <adc.h>
#include <FreeRTOS.h>
#include <task.h>
extern const struct temp_ops ntc_temp_ops;
struct temp_ntc {
	struct temp_generic gen;
	struct adc *adc;
	float Tn;
	float B;
	float Rn;
	int32_t adcmax;
};
/**\endcond*/
int32_t ntc_temp_connect(struct temp *temp, struct adc *adc);
#define TEMP_NTC(id, tn, b, rn, _adcmax) \
	struct temp_ntc ntc_##id = { \
		TEMP_INIT_DEV(ntc) \
		HAL_NAME("NTC " #id) \
		.adc = NULL, \
		.Tn = tn, \
		.B = b, \
		.Rn = rn, \
		.adcmax = _adcmax, \
	}; \
	TEMP_ADDDEV(ntc, ntc_##id)
#define TEMP_NTC_ID(id) HAL_GET_ID(temp, ntc, ntc_##id)
#endif
