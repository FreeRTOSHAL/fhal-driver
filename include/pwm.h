#ifndef PWM_H_
#define PWM_H_

#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
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
#ifdef CONFIG_PWM_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct pwm_ops *ops;
#endif
};
/**
 * Global container of all PWM instances
 */
extern struct pwm **pwms;
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
/**\}*/
#endif
