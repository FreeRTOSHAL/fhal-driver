#ifndef PWM_H_
#define PWM_H_

#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>

struct pwm;
#ifdef CONFIG_PWM_MULTI
struct pwm_ops {
	struct pwm *(*pwm_init)(uint32_t index);
	int32_t (*pwm_deinit)(struct pwm *pwm);

	int32_t (*pwm_setPeriod)(struct pwm *pwm, uint64_t us);
	int32_t (*pwm_setDutyCycle)(struct pwm *pwm, uint64_t us);
};
#endif
struct pwm_generic {
	bool init;
#ifdef CONFIG_UART_MULTI
	const struct pwm_ops *ops;
#endif
};
extern struct pwm **pwms;
#ifndef CONFIG_PWM_MULTI
struct pwm *pwm_init(uint32_t index);
int32_t pwm_deinit(struct pwm *pwm);

int32_t pwm_setPeriod(struct pwm *pwm, uint64_t us);
int32_t pwm_setDutyCycle(struct pwm *pwm, uint64_t us);
#else
inline struct pwm *pwm_init(uint32_t index) {
	struct pwm_generic *p = (struct pwm_generic *) pwms[index];
	return p->ops->pwm_init(index);
}
inline int32_t pwm_deinit(struct pwm *pwm) {
	struct pwm_generic *p = (struct pwm_generic *) pwm;
	return p->ops->pwm_deinit(pwm);
}

inline int32_t pwm_setPeriod(struct pwm *pwm, uint64_t us) {
	struct pwm_generic *p = (struct pwm_generic *) pwm;
	return p->ops->pwm_setPeriod(pwm, us);
}
inline int32_t pwm_setDutyCycle(struct pwm *pwm, uint64_t us) {
	struct pwm_generic *p = (struct pwm_generic *) pwm;
	return p->ops->pwm_setDutyCycle(pwm, us);
}
#endif
#endif
