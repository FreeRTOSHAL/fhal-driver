#ifndef PWM_PRV_
#define PWM_PRV_
#ifndef PWM_PRV
# error "Never include this file out of a PWM driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define PWM_ALREDY_INITED 1
int32_t pwm_generic_init(struct pwm *pwm);
#define PWM_ADDDEV(ns, p) HAL_ADDDEV(pwm, ns, p)
#ifndef CONFIG_PWM_MULTI
# define PWM_OPS(ns)
# define PWM_INIT_DEV(ns) 

# define PWM_INIT(ns, index) struct pwm *pwm_init(int32_t index)
# define PWM_DEINIT(ns, p) int32_t pwm_deinit(struct pwm *p)

# define PWM_SET_PERIOD(ns, p, us) int32_t pwm_setPeriod(struct pwm *p, uint64_t us)
# define PWM_SET_DUTY_CYCLE(ns, p, us) int32_t pwm_setDutyCycle(struct pwm *p, uint64_t us)
#else
# define PWM_OPS(ns) static const struct pwm_ops ns##_pwm_ops = { \
	.pwm_init = &ns##_pwm_init,\
	.pwm_deinit = &ns##_pwm_deinit,\
	.pwm_setPeriod = &ns##_pwm_setPeriod, \
	.pwm_setDutyCycle = &ns##_pwm_setDutyCycle, \
}
# define PWM_INIT_DEV(ns) .gen.ops = &ns##_pwm_ops,

# define PWM_INIT(ns, index) struct pwm *ns##_pwm_init(int32_t index)
# define PWM_DEINIT(ns, p) int32_t ns##_pwm_deinit(struct pwm *p)

# define PWM_SET_PERIOD(ns, p, us) int32_t ns##_pwm_setPeriod(struct pwm *p, uint64_t us)
# define PWM_SET_DUTY_CYCLE(ns, p, us) int32_t ns##_pwm_setDutyCycle(struct pwm *p, uint64_t us)
#endif
#endif
