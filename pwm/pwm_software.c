#include <pwm.h>
#define PWM_PRV
#include <pwm_prv.h>
#include <pwm_software.h>
#include <timer.h>
#include <gpio.h>
#ifdef CONFIG_PWM_SOFTWARE_DEBUG
# define PRINTF(fmt, ...) printf("Soft PWM: " fmt, ##__VA_ARGS__)
# define PRINTF_ISR(i, fmt, ...) do {if(i++ == 50) {i = 0; PRINTF(fmt, ##__VA_ARGS__);}} while(0)
uint32_t i_set = 0;
uint32_t i_clear = 0;
#else
# define PRINTF(fmt, ...)
# define PRINTF_ISR(i, fmt, ...)
#endif

static bool pwm_software_timerCallback(struct timer *timer, void *data) {
	int32_t ret;
	struct pwm_software *pwm = data;
	uint64_t value;
	if (pwm->oldvalue) {
		ret = gpioPin_clearPin(pwm->pin);
		CONFIG_ASSERT(ret >= 0);
		value = pwm->period - pwm->duty_cycle;
		PRINTF_ISR(i_set, "Clear Pin and set Waittime to %llu\n", value);
		ret = timer_oneshot(pwm->timer, value);
		CONFIG_ASSERT(ret >= 0);
	} else {
		ret = gpioPin_setPin(pwm->pin);
		CONFIG_ASSERT(ret >= 0);
		/**
		 * Update duty Cycle at begin of PWM
		 */
		if (pwm->updated) {
			pwm->duty_cycle = pwm->duty_cycleNew;
			pwm->updated = false;
		}
		PRINTF_ISR(i_clear, "Set Pin and set Waittime to %llu\n", pwm->duty_cycle);
		ret = timer_oneshot(pwm->timer, pwm->duty_cycle);
		CONFIG_ASSERT(ret >= 0);
	}

	pwm->oldvalue = !pwm->oldvalue;
	return false;
}

PWM_INIT(software, index) {
	int32_t ret;
	struct pwm_software *pwm;
	pwm = (struct pwm_software *) PWM_GET_DEV(index);
	if (pwm == NULL) {
		goto software_pwm_init_error0;
	}
	ret = pwm_generic_init((struct pwm *) pwm);
	if (ret < 0) {
		goto software_pwm_init_error0;
	}
	if (ret > 0) {
		goto software_pwm_init_exit;
	}
	pwm->oldvalue = true;
	pwm->period = 0;
	pwm->duty_cycle = 0;
	pwm->timer = NULL;
	pwm->pin = NULL;
	pwm->running = false;
software_pwm_init_exit:
	return (struct pwm *) pwm;
software_pwm_init_error0:
	return NULL;
}
int32_t pwm_software_connect(struct pwm *p, struct timer *timer, struct gpio_pin *pin) {
	int32_t ret;
	struct pwm_software *pwm = (struct pwm_software *) p;
	pwm->timer = timer;
	pwm->pin = pin;
	ret = timer_setOverflowCallback(pwm->timer, pwm_software_timerCallback, pwm);
	if (ret < 0) {
		return -1;
	}
	ret = gpioPin_setPin(pwm->pin);
	if (ret < 0) {
		return -1;
	}
	return 0;
}
PWM_DEINIT(software, p) {
	struct pwm_software *pwm = (struct pwm_software *) p;
	gpioPin_deinit(pwm->pin);
	timer_deinit(pwm->timer);
	pwm->gen.init = false;
	return 0;
}
PWM_SET_PERIOD(software, p, us) {
	struct pwm_software *pwm = (struct pwm_software *) p;
	int32_t ret; 
	ret = timer_stop(pwm->timer);
	if (ret < 0) {
		goto pwm_setPeriod_error0;
	}
	pwm->duty_cycle = 0;
	pwm->oldvalue = true;
	pwm->period = us;
	pwm->running = false;
	pwm->updated = false;
	ret = gpioPin_setPin(pwm->pin);
	if (ret < 0) {
		goto pwm_setPeriod_error0;
	}
	return 0;
pwm_setPeriod_error0:
	return -1;
}
PWM_SET_DUTY_CYCLE(software, p, us) {
	struct pwm_software *pwm = (struct pwm_software *) p;
	uint32_t ret;
	if (!pwm->period) {
		goto software_pwm_setDutyCycle_error0;
	}
	/* Stop Timer */
	if (us == 0) {
		PRINTF("Disable Timer Duty Cycle is 0\n");
		ret = gpioPin_clearPin(pwm->pin);
		if (ret < 0) {
			goto software_pwm_setDutyCycle_error0;
		}
		pwm->running = false;
		ret = timer_stop(pwm->timer);
		if (ret < 0) {
			goto software_pwm_setDutyCycle_error0;
		}
	} else if (pwm->running) {
		/* only update on change */
		if (pwm->duty_cycle != us) {
			PRINTF("Enable Timer Update Duty Cycle is %llu\n", us);
			pwm->updated = true;
			pwm->duty_cycleNew = us;
		}
	} else {
		PRINTF("Enable Timer Duty Cycle is %llu\n", us);
		pwm->duty_cycle = us;
		pwm->updated = false;
		pwm->running = true;
		pwm->oldvalue = true;
		ret = gpioPin_setPin(pwm->pin);
		if (ret < 0) {
			goto software_pwm_setDutyCycle_error0;
		}
		ret = timer_oneshot(pwm->timer, pwm->duty_cycle);
		if (ret < 0) {
			goto software_pwm_setDutyCycle_error0;
		}
	}
	return 0;
software_pwm_setDutyCycle_error0:
	return -1;
}
PWM_OPS(software);
