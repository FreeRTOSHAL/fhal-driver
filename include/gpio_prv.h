/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef GPIO_PRV_H_
#define GPIO_PRV_H_
#ifndef GPIO_PRV
# error "Never include this file out of a GPIO driver"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <hal.h>
#define GPIO_ALREDY_INITED 1
int32_t gpio_genericInit(struct gpio *gpio);
#define GPIO_ADDDEV(ns, p) HAL_ADDDEV(gpio, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(gpio);
#define GPIO_GET_DEV(index) HAL_GET_DEV(gpio, index)
#ifndef CONFIG_GPIO_MULTI
# define GPIO_OPS(ns)
# define GPIO_INIT_DEV(ns) 
# define GPIO_INIT(ns, index) struct gpio *gpio_init(uint32_t index)
# define GPIO_DEINIT(ns, g) int32_t gpio_deinit(struct gpio *g)
# define GPIO_PIN_INIT(ns, g, p, dir, setting) struct gpio_pin *gpioPin_init(struct gpio *g, uint32_t p, enum gpio_direction dir, enum gpio_setting setting)
# define GPIO_PIN_DEINIT(ns, p) int32_t gpioPin_deinit(struct gpio_pin *p)
# define GPIO_PIN_ENABLE_INTERRUPT(ns, p) int32_t gpioPin_enableInterrupt(struct gpio_pin *p)
# define GPIO_PIN_DISABLE_INTERRUPT(ns, p) int32_t gpioPin_disableInterrupt(struct gpio_pin *p)
# define GPIO_PIN_SET_CALLBACK(ns, p, callback, data, inter) int32_t gpioPin_setCallback(struct gpio_pin *p, bool (*callback)(struct gpio_pin *pin, uint32_t pinID, void *data), void *data, enum gpio_interrupt inter)
# define GPIO_PIN_SET_DIRECTION(ns, p, dir) int32_t gpioPin_setDirection(struct gpio_pin *p, enum gpio_direction dir)
# define GPIO_PIN_SET_SETTING(ns, p, setting) int32_t gpioPin_setSetting(struct gpio_pin *p, enum gpio_setting setting)
# define GPIO_PIN_SCHMITT_TRIGGER(ns, p, schmitt) int32_t gpioPin_SchmittTrigger(struct gpio_pin *p, bool schmitt)
# define GPIO_PIN_SET_VALUE(ns, p, value) int32_t gpioPin_setValue(struct gpio_pin *p, bool value)
# define GPIO_PIN_SET_PIN(ns, p) int32_t gpioPin_setPin(struct gpio_pin *p)
# define GPIO_PIN_CLEAR_PIN(ns, p) int32_t gpioPin_clearPin(struct gpio_pin *p)
# define GPIO_PIN_TOGGLE_PIN(ns, p) int32_t gpioPin_togglePin(struct gpio_pin *p)
# define GPIO_PIN_GET_VALUE(ns, p) bool gpioPin_getValue(struct gpio_pin *p)
#else
# define GPIO_OPS(ns) const struct gpio_ops ns##_gpio_ops = { \
	.gpio_init = ns##_gpio_init, \
	.gpio_deinit = ns##_gpio_deinit, \
	.gpioPin_init = ns##_gpioPin_init, \
	.gpioPin_deinit = ns##_gpioPin_deinit, \
	.gpioPin_enableInterrupt = ns##_gpioPin_enableInterrupt, \
	.gpioPin_disableInterrupt = ns##_gpioPin_disableInterrupt, \
	.gpioPin_setCallback = ns##_gpioPin_setCallback, \
	.gpioPin_setDirection = ns##_gpioPin_setDirection, \
	.gpioPin_setSetting = ns##_gpioPin_setSetting, \
	.gpioPin_SchmittTrigger = ns##_gpioPin_SchmittTrigger, \
	.gpioPin_setValue = ns##_gpioPin_setValue, \
	.gpioPin_setPin = ns##_gpioPin_setPin, \
	.gpioPin_clearPin = ns##_gpioPin_clearPin, \
	.gpioPin_togglePin = ns##_gpioPin_togglePin, \
	.gpioPin_getValue = ns##_gpioPin_getValue \
}
# define GPIO_INIT_DEV(ns) .gen.ops = &ns##_gpio_ops,
# define GPIO_INIT(ns, index) static struct gpio *ns##_gpio_init(uint32_t index)
# define GPIO_DEINIT(ns, g) static int32_t ns##_gpio_deinit(struct gpio *g)
# define GPIO_PIN_INIT(ns, g, p, dir, setting) static struct gpio_pin *ns##_gpioPin_init(struct gpio *g, uint32_t p, enum gpio_direction dir, enum gpio_setting setting)
# define GPIO_PIN_DEINIT(ns, p) static int32_t ns##_gpioPin_deinit(struct gpio_pin *p)
# define GPIO_PIN_ENABLE_INTERRUPT(ns, p) static int32_t ns##_gpioPin_enableInterrupt(struct gpio_pin *p)
# define GPIO_PIN_DISABLE_INTERRUPT(ns, p) static int32_t ns##_gpioPin_disableInterrupt(struct gpio_pin *p)
# define GPIO_PIN_SET_CALLBACK(ns, p, callback, data, inter) static int32_t ns##_gpioPin_setCallback(struct gpio_pin *p, bool (*callback)(struct gpio_pin *pin, uint32_t pinID, void *data), void *data, enum gpio_interrupt inter)
# define GPIO_PIN_SET_DIRECTION(ns, p, dir) static int32_t ns##_gpioPin_setDirection(struct gpio_pin *p, enum gpio_direction dir)
# define GPIO_PIN_SET_SETTING(ns, p, setting) static int32_t ns##_gpioPin_setSetting(struct gpio_pin *p, enum gpio_setting setting)
# define GPIO_PIN_SCHMITT_TRIGGER(ns, p, schmitt) static int32_t ns##_gpioPin_SchmittTrigger(struct gpio_pin *p, bool schmitt)
# define GPIO_PIN_SET_VALUE(ns, p, value) static int32_t ns##_gpioPin_setValue(struct gpio_pin *p, bool value)
# define GPIO_PIN_SET_PIN(ns, p) static int32_t ns##_gpioPin_setPin(struct gpio_pin *p)
# define GPIO_PIN_CLEAR_PIN(ns, p) static int32_t ns##_gpioPin_clearPin(struct gpio_pin *p)
# define GPIO_PIN_TOGGLE_PIN(ns, p) int32_t ns##_gpioPin_togglePin(struct gpio_pin *p)
# define GPIO_PIN_GET_VALUE(ns, p) static bool ns##_gpioPin_getValue(struct gpio_pin *p)
#endif
#endif
