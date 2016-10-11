#ifndef PWM_SOFTWARE_H_
#define PWM_SOFTWARE_H_
/**\cond INTERNAL*/
#include <pwm.h>
#define PWM_PRV
#include <pwm_prv.h>
#include <gpio.h>
extern const struct pwm_ops software_pwm_ops;
struct pwm_software {
	struct pwm_generic gen;
	struct timer *timer;
	struct gpio_pin *pin;
	uint64_t period;
	uint64_t duty_cycle;
	uint64_t duty_cycleNew;
	bool updated;
	bool running;
	bool oldvalue;
};
/**\endcond*/
int32_t pwm_software_connect(struct pwm *pwm, struct timer *timer, struct gpio_pin *pin);
#define ADD_PWM_SOFTWARE(id) \
	struct pwm_software pwm_software_##id = { \
		PWM_INIT_DEV(software) \
		HAL_NAME("Software PWM " #id) \
	};\
	PWM_ADDDEV(software, pwm_software_##id)
#define PWM_SOFTWARE_ID(id) HAL_GET_ID(pwm, software, pwm_software_##id)
#endif
