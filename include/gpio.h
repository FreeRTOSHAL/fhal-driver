#ifndef GPIO_H_
#define GPIO_H_
#include <stdint.h>
#include <stdbool.h>
#include <mux.h> 
struct gpio;
struct gpio_pin;

enum gpio_direction {
	GPIO_INPUT,
	GPIO_OUTPUT
};

enum gpio_setting {
	GPIO_OPEN,
	GPIO_PULL_UP,
	GPIO_PULL_DOWN
};

enum gpio_interrupt  {
	GPIO_FALLING,
	GPIO_RISING,
	GPIO_EITHER
};

struct gpio *gpio_init();
int32_t gpio_deinit(struct gpio *gpio);

struct gpio_pin *gpioPin_init(struct gpio *gpio, uint8_t pin, enum gpio_direction dir, enum gpio_setting setting);
int32_t gpioPin_deinit(struct gpio_pin *pin);
int32_t gpioPin_enableInterrupt(struct gpio_pin *pin);
int32_t gpioPin_disableInterrupt(struct gpio_pin *pin);
int32_t gpioPin_setCallback(struct gpio_pin *pin, bool (*calback)(struct gpio_pin *pin, void *data), void *data, enum gpio_interrupt inter);
int32_t gpioPin_setDirection(struct gpio_pin *pin, enum gpio_direction dir);
int32_t gpioPin_setSetting(struct gpio_pin *pin, enum gpio_setting setting);
int32_t gpioPin_SchmittTrigger(struct gpio_pin *pin, bool schmit);
int32_t gpioPin_setValue(struct gpio_pin *pin, bool value);
int32_t gpioPin_setPin(struct gpio_pin *pin);
int32_t gpioPin_clearPin(struct gpio_pin *pin);
int32_t gpioPin_togglePin(struct gpio_pin *pin);
bool gpioPin_getValue(struct gpio_pin *pin);

#endif
