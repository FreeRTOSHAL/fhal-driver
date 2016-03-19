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
#include <hal.h>
#include <stdio.h>
int32_t hal_init(void *data);
int32_t hal_deinit(void *data);
bool hal_isInit(void *data);
int32_t hal_lock(void *data, TickType_t waittime);
int32_t hal_unlock(void *data);
uintptr_t *hal_getDev(uintptr_t **devs, uintptr_t **end, uint32_t index);
#ifdef CONFIG_INSTANCE_NAME
#define printNameOfDriver(gns) { \
	HAL_DEFINE_GLOBAL_ARRAY(gns); \
	uint32_t i = 0; \
	uintptr_t **instances; \
	struct hal *hal; \
	if (&_dev_##gns != &_dev_##gns##_end) { \
		printf("%s:\n", #gns); \
		for(instances = &_dev_##gns; instances < &_dev_##gns##_end;instances++, i++) { \
			hal = (struct hal *) *instances; \
			printf("%ld: %s\n", i, hal->name); \
		} \
	} \
}
int32_t hal_printNames() {
	printNameOfDriver(hal);
	printNameOfDriver(gpio);
	printNameOfDriver(uart);
	printNameOfDriver(timer);
	printNameOfDriver(pwm);
	printNameOfDriver(capture);
	printNameOfDriver(spi);
	printNameOfDriver(accel);
	printNameOfDriver(gyro);
	printNameOfDriver(adc);
	printNameOfDriver(example);
	return 0;
}
#endif
