/*
 * Copyright (c) 2019 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
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
