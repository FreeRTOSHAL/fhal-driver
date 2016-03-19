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
#ifndef PWM_H_
#define PWM_H_

#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <hal.h>
/**
 * \defgroup PWM PWM Subsystem
 * \ingroup HAL
 * \code
 * #include <pwm.h>
 * \endcode
 *
 * This is the PWM Subsystem for controlling PWM of a SOC. 
 * 
 * The most PWM Driver need a Timer! See {@link TIMER} Interface
 * \{
 */
/**
 * Private Structure of PWM driver
 */
struct pwm;
#ifdef CONFIG_PWM_MULTI
/**
 * Function of timer driver in Multi Driver implementation mode 
 */
struct pwm_ops {
	struct pwm *(*pwm_init)(uint32_t index);
	int32_t (*pwm_deinit)(struct pwm *pwm);

	int32_t (*pwm_setPeriod)(struct pwm *pwm, uint64_t us);
	int32_t (*pwm_setDutyCycle)(struct pwm *pwm, uint64_t us);
};
#endif
/**
 * Generic PWM Interface  
 */
struct pwm_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_INSTANCE_NAME
	/**
	 * Name of Driver Instance for Debugging 
	 */
	const char *name;
#endif
#ifdef CONFIG_PWM_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct pwm_ops *ops;
#endif
};
#ifndef CONFIG_PWM_MULTI
/**
 * Init PWM instances 
 * \param index Index of PWM
 * \return PWM Instance NULL on Error
 */
struct pwm *pwm_init(uint32_t index);
/**
 * Deinit PWM
 * \param pwm PWM instance
 * \return -1 on error 0 on ok
 */
int32_t pwm_deinit(struct pwm *pwm);
/**
 * Set Period of PWM
 * \code{.unparsed}
 * |  ______________           ______________              
 * |  |            |           |            | 
 * |  |            |           |            |
 * |__|            |___________|            | 
 * +-------------------------------------------- 
 *    |          Period        | 
 *    | Duty Cycle | 
 * \endcode
 * \param pwm PWM instance
 * \param us Time
 * \return -1 on error 0 on ok
 */
int32_t pwm_setPeriod(struct pwm *pwm, uint64_t us);
/**
 * Set Duty Cycle of PWM
 * \code{.unparsed}
 * |  ______________           ______________              
 * |  |            |           |            | 
 * |  |            |           |            |
 * |__|            |___________|            | 
 * +-------------------------------------------- 
 *    |          Period        | 
 *    | Duty Cycle | 
 * \endcode
 * \param pwm PWM instance
 * \param us Time
 * \return -1 on error 0 on ok
 */
int32_t pwm_setDutyCycle(struct pwm *pwm, uint64_t us);
#else
inline struct pwm *pwm_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(pwm);
	struct pwm_generic *p = (struct pwm_generic *) HAL_GET_DEV(pwm, index);
	if (p == NULL) {
		return NULL;
	}
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
/**\}*/
#endif
