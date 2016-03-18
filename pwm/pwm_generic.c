#include <pwm.h>
#define PWM_PRV
#include <pwm_prv.h>

int32_t pwm_generic_init(struct pwm *t) {
	struct pwm_generic *pwm = (struct pwm_generic *) t;
	if (hal_isInit(pwm)) {
		return -PWM_ALREDY_INITED;
	}
	pwm->init = true;
	return 0;
}
#ifdef CONFIG_PWM_MULTI
struct pwm *pwm_init(uint32_t index);
int32_t pwm_deinit(struct pwm *pwm);

int32_t pwm_setPeriod(struct pwm *pwm, uint64_t us);
int32_t pwm_setDutyCycle(struct pwm *pwm, uint64_t us);
#endif
