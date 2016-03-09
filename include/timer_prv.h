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
#define TIMER_OPS(ns) static const struct timer_ops ns##ops = { \
	.timer_init = &ns##_timer_init, \
	.timer_deinit = &ns##_timer_deinit, \
	.timer_setOverflowCallback = &ns##_timer_setOverflowCallback, \
	.timer_start = &ns##_timer_start, \
	.timer_stop = &ns##_timer_stop, \
	.timer_oneshot = &ns##_timer_oneshot, \
	.timer_periodic = &ns##_timer_periodic, \
	.timer_getTime = &ns##_timer_getTime, \
}
# define TIMER_INIT_DEV(ns) .gen.ops = &ns##ops,
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
