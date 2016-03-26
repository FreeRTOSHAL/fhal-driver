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
#include <timer.h>
#define TIMER_PRV
#include <timer_prv.h>

int32_t timer_generic_init(struct timer *t) {
	struct timer_generic *timer = (struct timer_generic *) t;
	if (hal_isInit(timer)) {
		return TIMER_ALREDY_INITED;
	}
	timer->init = true;
	return 0;
}
#ifdef CONFIG_TIMER_MULTI
struct timer *timer_init(uint32_t index, uint32_t prescaler, uint64_t basetime, int64_t adjust);
int32_t timer_deinit(struct timer *timer);

int32_t timer_setOverflowCallback(struct timer *timer, bool (*callback)(struct timer *timer, void *data), void * data);
int32_t timer_start(struct timer *timer);
int32_t timer_stop(struct timer *timer);
int32_t timer_oneshot(struct timer *timer, uint64_t us);
int32_t timer_periodic(struct timer *timer, uint64_t us);
uint64_t timer_getTime(struct timer *timer);
#endif
