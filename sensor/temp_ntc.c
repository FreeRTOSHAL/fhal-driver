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
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <temp.h>
#include <temp_ntc.h>
#include <temp_prv.h>
#include <adc.h>
#include <math.h>

int32_t ntc_temp_connect(struct temp *t, struct adc *adc) {
	struct temp_ntc *temp = (struct temp_ntc *) t;
	if (!temp->gen.init) {
		return -1;
	}
	if (adc == NULL) {
		return -1;
	}
	temp->adc = adc;
	return 0;
}

TEMP_INIT(ntc, index) {
	struct temp_ntc *temp;
	int32_t ret;
	temp = (struct temp_ntc *) TEMP_GET_DEV(index);
	if (temp == NULL) {
		goto temp_ntc_init_error0;
	}
	ret = temp_generic_init((struct temp *) temp);
	if (ret < 0) {
		goto temp_ntc_init_error0;
	}
	if (ret > 0) {
		goto temp_ntc_init_exit;
	}
	temp->gen.init = true;
temp_ntc_init_exit:
	return (struct temp *) temp;
temp_ntc_init_error0:
	return NULL;
}

TEMP_DEINIT(ntc, t) {
	struct temp_ntc *temp = (struct temp_ntc *) t;
	temp->adc = NULL;
	temp->gen.init = false;
	return 0;
}
static float temp_ntc_calcValue(struct temp_ntc *temp, int32_t value) {
	return (1. / (
			(
				1. / temp->Tn /*K*/
			) + (
				(
					1. / temp->B /* B */
				) * (
					log( 
						(
							/* calc Rt */
							(value * temp->Rn) / ((float) (temp->adcmax - value))
						) / temp->Rn
					)
				)
			)
		)
	) - 273.15 /* Kelvin to Celsius */;
}

TEMP_GET(ntc, t, value, waittime) {
	struct temp_ntc *temp = (struct temp_ntc *) t;
	int32_t v;
	if (!temp->adc) {
		return -1;
	}
	if (!value) {
		return -1;
	}
	temp_lock(t, waittime, -1);
	v = adc_get(temp->adc, waittime);
	if (v < 0) {
		return -1;
	}
	temp_unlock(t, -1);
	*value = temp_ntc_calcValue(temp, v);
	return 0;
}

TEMP_GET_ISR(ntc, t, value) {
	struct temp_ntc *temp = (struct temp_ntc *) t;
	int32_t v;
	if (!temp->adc) {
		return -1;
	}
	if (!value) {
		return -1;
	}
	v = adc_getISR(temp->adc);
	if (v < 0) {
		return -1;
	}
	*value = temp_ntc_calcValue(temp, v);
	return 0;
}
TEMP_OPS(ntc);
