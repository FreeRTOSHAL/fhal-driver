/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
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
