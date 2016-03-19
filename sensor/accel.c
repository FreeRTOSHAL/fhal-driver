/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
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
#include <accel.h>
#define ACCEL_PRV
#include <accel_prv.h>


int32_t accel_generic_init(struct accel *accel) {
	struct accel_generic * s = (struct accel_generic *) accel;
	if(hal_isInit(s)) {
		return ACCEL_ALREDY_INITED;
	}
#ifdef CONFIG_ACCEL_THREAD_SAVE
	{
		int32_t ret = hal_init(s);
		if (ret < 0) {
			goto accel_genericInit_error0;
		}
	}
#endif
	s->init = true;
	return 0;
#ifdef CONFIG_ACCEL_THREAD_SAVE
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
