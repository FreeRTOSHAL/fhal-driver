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
	return ((temp->B * temp->Tn) / 
		(
			temp->B + log(
				/* calc Rt */
				(value * temp->Rn) / ((float) (temp->adcmax - value)) / temp->Rn
			) * temp->Tn
		)
	) - 273.15;
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
