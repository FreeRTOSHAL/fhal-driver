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
HAL_DEFINE_GLOBAL_ARRAY(pwm);
#define PWM_GET_DEV(index) HAL_GET_DEV(pwm, index)
#ifndef CONFIG_PWM_MULTI
# define PWM_OPS(ns)
# define PWM_INIT_DEV(ns) 

# define PWM_INIT(ns, index) struct pwm *pwm_init(uint32_t index)
# define PWM_DEINIT(ns, p) int32_t pwm_deinit(struct pwm *p)

# define PWM_SET_PERIOD(ns, p, us) int32_t pwm_setPeriod(struct pwm *p, uint64_t us)
# define PWM_SET_DUTY_CYCLE(ns, p, us) int32_t pwm_setDutyCycle(struct pwm *p, uint64_t us)
#else
# define PWM_OPS(ns) const struct pwm_ops ns##_pwm_ops = { \
	.pwm_init = &ns##_pwm_init,\
	.pwm_deinit = &ns##_pwm_deinit,\
	.pwm_setPeriod = &ns##_pwm_setPeriod, \
	.pwm_setDutyCycle = &ns##_pwm_setDutyCycle, \
}
# define PWM_INIT_DEV(ns) .gen.ops = &ns##_pwm_ops,

# define PWM_INIT(ns, index) struct pwm *ns##_pwm_init(uint32_t index)
# define PWM_DEINIT(ns, p) int32_t ns##_pwm_deinit(struct pwm *p)

# define PWM_SET_PERIOD(ns, p, us) int32_t ns##_pwm_setPeriod(struct pwm *p, uint64_t us)
# define PWM_SET_DUTY_CYCLE(ns, p, us) int32_t ns##_pwm_setDutyCycle(struct pwm *p, uint64_t us)
#endif
#endif
