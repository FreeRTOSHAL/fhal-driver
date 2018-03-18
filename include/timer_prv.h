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
#ifndef TIMER_PRV_
#define TIMER_PRV_
#ifndef TIMER_PRV
# error "Never include this file out of a Timer driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define TIMER_ALREDY_INITED 1
int32_t timer_generic_init(struct timer *timer);
#define TIMER_ADDDEV(ns, p) HAL_ADDDEV(timer, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(timer);
#define TIMER_GET_DEV(index) HAL_GET_DEV(timer, index)
#ifndef CONFIG_TIMER_MULTI
#define TIMER_OPS(ns)
# define TIMER_INIT_DEV(ns) 
#define TIMER_INIT(ns, index, prescaler, basetime, adjust) struct timer *timer_init(uint32_t index, uint32_t prescaler, uint64_t basetime, int64_t adjust)
#define TIMER_DEINIT(ns, t) int32_t timer_deinit(struct timer *t)

#define TIMER_SET_OVERFLOW_CALLBACK(ns, t, callback, data) int32_t timer_setOverflowCallback(struct timer *t, bool (*callback)(struct timer *timer, void *d), void * data)
#define TIMER_START(ns, t) int32_t timer_start(struct timer *t)
#define TIMER_STOP(ns, t) int32_t timer_stop(struct timer *t)
#define TIMER_ONESHOT(ns, t, us) int32_t timer_oneshot(struct timer *t, uint64_t us)
#define TIMER_PERIODIC(ns, t, us) int32_t timer_periodic(struct timer *t, uint64_t us)
#define TIMER_GET_TIME(ns, t) uint64_t timer_getTime(struct timer *t)
#else
#define TIMER_OPS(ns) const struct timer_ops ns##_timer_ops = { \
	.timer_init = &ns##_timer_init, \
	.timer_deinit = &ns##_timer_deinit, \
	.timer_setOverflowCallback = &ns##_timer_setOverflowCallback, \
	.timer_start = &ns##_timer_start, \
	.timer_stop = &ns##_timer_stop, \
	.timer_oneshot = &ns##_timer_oneshot, \
	.timer_periodic = &ns##_timer_periodic, \
	.timer_getTime = &ns##_timer_getTime, \
}
# define TIMER_INIT_DEV(ns) .gen.ops = &ns##_timer_ops,
#define TIMER_INIT(ns, index, prescaler, basetime, adjust) static struct timer *ns##_timer_init(uint32_t index, uint32_t prescaler, uint64_t basetime, int64_t adjust)
#define TIMER_DEINIT(ns, t) static int32_t ns##_timer_deinit(struct timer *t)

#define TIMER_SET_OVERFLOW_CALLBACK(ns, t, callback, data) static int32_t ns##_timer_setOverflowCallback(struct timer *t, bool (*callback)(struct timer *timer, void *d), void * data)
#define TIMER_START(ns, t) static int32_t ns##_timer_start(struct timer *t)
#define TIMER_STOP(ns, t) static int32_t ns##_timer_stop(struct timer *t)
#define TIMER_ONESHOT(ns, t, us) static int32_t ns##_timer_oneshot(struct timer *t, uint64_t us)
#define TIMER_PERIODIC(ns, t, us) static int32_t ns##_timer_periodic(struct timer *t, uint64_t us)
#define TIMER_GET_TIME(ns, t) static uint64_t ns##_timer_getTime(struct timer *t)
#endif
#endif
