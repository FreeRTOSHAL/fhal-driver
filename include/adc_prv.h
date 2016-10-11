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
#ifndef ADC_PRV_H_
#define ADC_PRV_H_
#ifndef ADC_PRV
# error "Never include this file out of a ADC driver"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>
#include <stdint.h>
#include <hal.h>
#define ADC_ALREDY_INITED 1
int32_t adc_generic_init(struct adc *adc);
#ifdef CONFIG_ADC_THREAD_SAVE
# define adc_lock(u, w, r) HAL_LOCK(u, w, r)
# define adc_unlock(u, r) HAL_UNLOCK(u, r)
#else
# define adc_lock(u, w, r)
# define adc_unlock(u, r)
#endif
#define ADC_ADDDEV(ns, p) HAL_ADDDEV(adc, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(adc);
#define ADC_GET_DEV(index) HAL_GET_DEV(adc, index)
#ifndef CONFIG_ADC_MULTI
# define ADC_OPS(ns)
# define ADC_INIT_DEV(ns) 
#define ADC_INIT(ns, index, bits, hz) struct adc *adc_init(uint32_t index, uint8_t bits, uint32_t hz)
#define ADC_DEINIT(ns, a) int32_t adc_deinit(struct adc *a)
#define ADC_GET(ns, a, waittime) int32_t adc_get(struct adc *a, TickType_t waittime)
#define ADC_GET_ISR(ns, a) int32_t adc_getISR(struct adc *a)
#define ADC_SET_CALLBACK(ns, a, callback, data) int32_t adc_setCallback(struct adc *a, bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data), void *data)
#define ADC_START(ns, a) int32_t adc_start(struct adc *a)
#define ADC_STOP(ns, a) int32_t adc_stop(struct adc *a)
#else
# define ADC_OPS(ns) const struct adc_ops ns##_ops = { \
	.adc_init = &ns##_adc_init, \
	.adc_deinit = &ns##_adc_deinit, \
	.adc_get = &ns##_adc_get, \
	.adc_getISR = &ns##_adc_getISR, \
	.adc_setCallback = &ns##_adc_setCallback, \
	.adc_start = &ns##_adc_start, \
	.adc_stop = &ns##_adc_stop, \
}
# define ADC_INIT_DEV(ns) .gen.ops = &ns##_ops,

#define ADC_INIT(ns, index, bits, hz) static struct adc *ns##_adc_init(uint32_t index, uint8_t bits, uint32_t hz)
#define ADC_DEINIT(ns, a) static int32_t ns##_adc_deinit(struct adc *a)
#define ADC_GET(ns, a, waittime) static int32_t ns##_adc_get(struct adc *a, TickType_t waittime)
#define ADC_GET_ISR(ns, a) static int32_t ns##_adc_getISR(struct adc *a)
#define ADC_SET_CALLBACK(ns, a, callback, data) static int32_t ns##_adc_setCallback(struct adc *a, bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data), void *data)
#define ADC_START(ns, a) static int32_t ns##_adc_start(struct adc *a)
#define ADC_STOP(ns, a) static int32_t ns##_adc_stop(struct adc *a)
#endif
#endif
