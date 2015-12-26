#ifndef TIMER_H_
#include <stdbool.h>
#include <stdint.h>
struct timer;
struct timer_generic {
	bool init;
#ifdef CONFIG_TIMER_MULTI
	const struct timer_ops *ops;
#endif
};
#ifdef CONFIG_TIMER_MULTI
struct timer_ops {
	struct timer *(*timer_init)(uint32_t index, uint32_t prescaler, uint64_t basetime, uint64_t adjust);
	int32_t (*timer_deinit)(struct timer *timer);

	int32_t (*timer_setOverflowCallback)(struct timer *timer, bool (*callback)(struct timer *timer, void *data), void * data);
	int32_t (*timer_start)(struct timer *timer);
	int32_t (*timer_stop)(struct timer *timer);
	int32_t (*timer_oneshot)(struct timer *timer, uint64_t us);
	int32_t (*timer_periodic)(struct timer *timer, uint64_t us);
	int64_t (*timer_getTime)(struct timer *timer);
};
#endif
extern struct timer **timers;
#ifndef CONFIG_TIMER_MULTI
struct timer *timer_init(uint32_t index, uint32_t prescaler, uint64_t basetime, uint64_t adjust);
int32_t timer_deinit(struct timer *timer);

int32_t timer_setOverflowCallback(struct timer *timer, bool (*callback)(struct timer *timer, void *data), void * data);
int32_t timer_start(struct timer *timer);
int32_t timer_stop(struct timer *timer);
int32_t timer_oneshot(struct timer *timer, uint64_t us);
int32_t timer_periodic(struct timer *timer, uint64_t us);
int64_t timer_getTime(struct timer *timer);
#else
inline struct timer *timer_init(uint32_t index, uint32_t prescaler, uint64_t basetime, uint64_t adjust) {
	struct timer_generic *timer = (struct timer_generic *)timers[index];
	return timer->ops->timer_init(index, prescaler, basetime, adjust);
}
inline int32_t timer_deinit(struct timer *timer) {
	struct timer_generic *t = (struct timer_generic *) timer;
	return t->ops->timer_deinit(timer);
}

inline int32_t timer_setOverflowCallback(struct timer *timer, bool (*callback)(struct timer *timer, void *data), void * data) {
	struct timer_generic *t = (struct timer_generic *) timer;
	return t->ops->timer_setOverflowCallback(timer, callback, data);
}
inline int32_t timer_start(struct timer *timer) {
	struct timer_generic *t = (struct timer_generic *) timer;
	return t->ops->timer_start(timer);
}
inline int32_t timer_stop(struct timer *timer) {
	struct timer_generic *t = (struct timer_generic *) timer;
	return t->ops->timer_stop(timer);
}
inline int32_t timer_oneshot(struct timer *timer, uint64_t us) {
	struct timer_generic *t = (struct timer_generic *) timer;
	return t->ops->timer_oneshot(timer, us);
}
inline int32_t timer_periodic(struct timer *timer, uint64_t us) {
	struct timer_generic *t = (struct timer_generic *) timer;
	return t->ops->timer_periodic(timer, us);
}
inline int64_t timer_getTime(struct timer *timer) {
	struct timer_generic *t = (struct timer_generic *) timer;
	return t->ops->timer_getTime(timer);
}
#endif
#endif
