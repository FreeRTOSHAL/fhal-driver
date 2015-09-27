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

struct gpio *gpio_init();
int32_t gpio_deinit(struct gpio *gpio);
struct gpio_pin *gpio_getPin(struct gpio *gpio, uint8_t pin, enum gpio_direction dir);
int32_t gpio_setDirection(struct gpio_pin *pin, enum gpio_direction dir);
/* TODO Pullup Output Strengs ... */
int32_t gpio_setPinValue(struct gpio_pin *pin, bool value);
int32_t gpio_setPin(struct gpio_pin *pin);
int32_t gpio_clearPin(struct gpio_pin *pin);
int32_t gpio_togglePin(struct gpio_pin *pin);
bool gpio_getPinValue(struct gpio_pin *pin);

#endif
