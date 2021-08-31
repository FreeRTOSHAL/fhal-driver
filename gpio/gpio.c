/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#include <gpio.h>
#define GPIO_PRV
#include <gpio_prv.h>

#include <semphr.h>
int32_t gpio_genericInit(struct gpio *g) {
	struct gpio_generic *gpio = (struct gpio_generic *) g;
	if (hal_isInit(gpio)) {
		return GPIO_ALREDY_INITED;
	}
	gpio->init = true;
#ifdef CONFIG_GPIO_THREAD_SAFE
	{
		int32_t ret = hal_init(gpio);
		if (ret < 0) {
			return -1;
		}
	}
#endif
	return 0;
}

#ifdef CONFIG_GPIO_MULTI
struct gpio *gpio_init(uint32_t index);
int32_t gpio_deinit(struct gpio *gpio);
struct gpio_pin *gpioPin_init(struct gpio *gpio, uint32_t pin, enum gpio_direction dir, enum gpio_setting setting);
int32_t gpioPin_deinit(struct gpio_pin *pin);
int32_t gpioPin_enableInterrupt(struct gpio_pin *pin);
int32_t gpioPin_disableInterrupt(struct gpio_pin *pin);
int32_t gpioPin_setCallback(struct gpio_pin *pin, bool (*callback)(struct gpio_pin *pin, uint32_t pinID, void *data), void *data, enum gpio_interrupt inter);
int32_t gpioPin_setDirection(struct gpio_pin *pin, enum gpio_direction dir);
int32_t gpioPin_setSetting(struct gpio_pin *pin, enum gpio_setting setting);
int32_t gpioPin_SchmittTrigger(struct gpio_pin *pin, bool schmitt);
int32_t gpioPin_setValue(struct gpio_pin *pin, bool value);
int32_t gpioPin_setPin(struct gpio_pin *pin);
int32_t gpioPin_clearPin(struct gpio_pin *pin);
int32_t gpioPin_togglePin(struct gpio_pin *pin);
bool gpioPin_getValue(struct gpio_pin *pin);
#endif
