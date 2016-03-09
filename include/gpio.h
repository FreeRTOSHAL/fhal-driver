#ifndef GPIO_H_
#define GPIO_H_
#include <stdint.h>
#include <stdbool.h>
#include <mux.h> 

/**
 * \defgroup GPIO GPIO Subsystem
 * \ingroup HAL
 * \code
 * #include <gpio.h>
 * \endcode
 * 
 * This is the GPIO Subsystem for controlling GPIO of a SOC. 
 * \{
 */
/**
 * Private Structure of GPIO driver
 */
struct gpio;
/**
 * Private Structure of GPIO Pin
 */
struct gpio_pin;
/** 
 * GPIO Direction
 * \ingroup GPIO_PIN
 */
enum gpio_direction {
	/**
	 * Input
	 */
	GPIO_INPUT,
	/**
	 * Output
	 */
	GPIO_OUTPUT
};
/**
 * GPIO Setting
 * \ingroup GPIO_PIN
 */
enum gpio_setting {
	/**
	 * Configure Pin without Pull down or Pull up
	 */
	GPIO_OPEN,
	/**
	 * Configure Pin with Pull Up
	 */
	GPIO_PULL_UP,
	/**
	 * Configure Pin with Pull Down
	 */
	GPIO_PULL_DOWN
};
/**
 * GPIO as Interrupt
 * \ingroup GPIO_PIN
 */
enum gpio_interrupt  {
	/**
	 * Call a Interrupt while Falling Edge
	 */
	GPIO_FALLING,
	/**
	 * Call a Interrupt while Falling Rising
	 */
	GPIO_RISING,
	/**
	 * Call a Interrupt while Falling and Rising Edge
	 */
	GPIO_EITHER
};

#ifdef CONFIG_GPIO_MULTI
/**
 * Function of GPIO driver in Multi Driver implementation mode 
 */
struct gpio_ops {
	struct gpio *(*gpio_init)(uint32_t index);
	int32_t (*gpio_deinit)(struct gpio *gpio);
	struct gpio_pin *(*gpioPin_init)(struct gpio *gpio, uint8_t pin, enum gpio_direction dir, enum gpio_setting setting);
	int32_t (*gpioPin_deinit)(struct gpio_pin *pin);
	int32_t (*gpioPin_enableInterrupt)(struct gpio_pin *pin);
	int32_t (*gpioPin_disableInterrupt)(struct gpio_pin *pin);
	int32_t (*gpioPin_setCallback)(struct gpio_pin *pin, bool (*callback)(struct gpio_pin *pin, uint8_t pinID, void *data), void *data, enum gpio_interrupt inter);
	int32_t (*gpioPin_setDirection)(struct gpio_pin *pin, enum gpio_direction dir);
	int32_t (*gpioPin_setSetting)(struct gpio_pin *pin, enum gpio_setting setting);
	int32_t (*gpioPin_SchmittTrigger)(struct gpio_pin *pin, bool schmitt);
	int32_t (*gpioPin_setValue)(struct gpio_pin *pin, bool value);
	int32_t (*gpioPin_setPin)(struct gpio_pin *pin);
	int32_t (*gpioPin_clearPin)(struct gpio_pin *pin);
	int32_t (*gpioPin_togglePin)(struct gpio_pin *pin);
	bool (*gpioPin_getValue)(struct gpio_pin *pin);
};
#endif
/**
 * Generic GPIO Interface  
 */
struct gpio_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_GPIO_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct gpio_ops *ops;
#endif
};
/**
 * Generic GPIO Pin Interface  
 */
struct gpio_pin_generic {
	struct gpio_generic *gpio;
};
/**
 * Global container of all GPIO instances
 */
extern struct gpio **gpios;

#ifndef CONFIG_GPIO_MULTI
/**
 * Init GPIO Subssystem
 * \return GPIO Handle
 */
struct gpio *gpio_init(uint32_t index);
/**
 * Deinit GPIO Subssystem 
 * \param gpio GPIO Handle
 * \return -1 on Error 0 on ok 
 */
int32_t gpio_deinit(struct gpio *gpio);

/**
 * \defgroup GPIO_PIN GPIO Pin Subsystem
 * \ingroup GPIO
 * \code
 * #include <gpio.h>
 * \endcode
 * 
 * This is the Pin GPIO Subsystem for controlling one GPIO pin of a SOC. 
 * \{
 */

/**
 * Init one GPIO Pin 
 * \param gpio GPIO Handle
 * \param pin Pin GPIO pin 
 * \param dir Direction 
 * \param setting Pin Settings
 * \return GPIO Pin Handle or NULL on error
 */
struct gpio_pin *gpioPin_init(struct gpio *gpio, uint8_t pin, enum gpio_direction dir, enum gpio_setting setting);
/**
 * Deinit Pin
 * \param pin GPIO Pin Handle
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_deinit(struct gpio_pin *pin);
/**
 * Enable Interrupt on pin
 * \param pin GPIO Pin Handle
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_enableInterrupt(struct gpio_pin *pin);
/**
 * Disable Interrupt on pin
 * \param pin GPIO Pin Handle
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_disableInterrupt(struct gpio_pin *pin);
/**
 * Set Interrupt Callback
 * \param pin GPIO Pin Handle
 * \param callback Callback
 * \param data Data transmitted to Callback
 * \param inter Interrupt Setting
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_setCallback(struct gpio_pin *pin, bool (*callback)(struct gpio_pin *pin, uint8_t pinID, void *data), void *data, enum gpio_interrupt inter);
/**
 * Set Direction
 * \param pin GPIO Pin Handle
 * \param dir Direction
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_setDirection(struct gpio_pin *pin, enum gpio_direction dir);
/**
 * Set Setting
 * \param pin GPIO Pin Handle
 * \param setting GPIO Pin Setting
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_setSetting(struct gpio_pin *pin, enum gpio_setting setting);
/**
 * Set Schmitt Trigger 
 * \param pin GPIO Pin Handle
 * \param schmitt true = activate Schmitt Trigger false = deactivate Schmitt Trigger  
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_SchmittTrigger(struct gpio_pin *pin, bool schmitt);
/**
 * Set Value of Pin
 * \param pin GPIO Pin Handle
 * \param value true = high false = low
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_setValue(struct gpio_pin *pin, bool value);
/**
 * Set High on Pin
 * \param pin GPIO Pin Handle
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_setPin(struct gpio_pin *pin);
/**
 * Set low on Pin 
 * \param pin GPIO Pin Handle
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_clearPin(struct gpio_pin *pin);
/**
 * Toggle Pin
 * \param pin GPIO Pin Handle
 * \return -1 on Error 0 on ok
 */
int32_t gpioPin_togglePin(struct gpio_pin *pin);
/**
 * Get Value
 * \param pin GPIO Pin Handle
 * \return -1 on Error 0 on ok
 */
bool gpioPin_getValue(struct gpio_pin *pin);
/**\}*/
#else
inline struct gpio *gpio_init(uint32_t index) {
	struct gpio_generic *gpio = (struct gpio_generic *) gpios[index];
	return gpio->ops->gpio_init(index);
}
inline int32_t gpio_deinit(struct gpio *gpio) {
	struct gpio_generic *g = (struct gpio_generic *) gpio;
	return g->ops->gpio_deinit(gpio);
}
inline struct gpio_pin *gpioPin_init(struct gpio *gpio, uint8_t pin, enum gpio_direction dir, enum gpio_setting setting) {
	struct gpio_generic *g = (struct gpio_generic *) gpio;
	return g->ops->gpioPin_init(gpio, pin, dir, setting);
}
inline int32_t gpioPin_deinit(struct gpio_pin *pin) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_deinit(pin);
}
inline int32_t gpioPin_enableInterrupt(struct gpio_pin *pin) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_enableInterrupt(pin);
}
inline int32_t gpioPin_disableInterrupt(struct gpio_pin *pin) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_disableInterrupt(pin);
}
inline int32_t gpioPin_setCallback(struct gpio_pin *pin, bool (*callback)(struct gpio_pin *pin, uint8_t pinID, void *data), void *data, enum gpio_interrupt inter) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_setCallback(pin, callback, data, inter);
}
inline int32_t gpioPin_setDirection(struct gpio_pin *pin, enum gpio_direction dir) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_setDirection(pin, dir);
}
inline int32_t gpioPin_setSetting(struct gpio_pin *pin, enum gpio_setting setting) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_setSetting(pin, setting);
}
inline int32_t gpioPin_SchmittTrigger(struct gpio_pin *pin, bool schmitt) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_SchmittTrigger(pin, schmitt);
}
inline int32_t gpioPin_setValue(struct gpio_pin *pin, bool value) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_setValue(pin, value);
}
inline int32_t gpioPin_setPin(struct gpio_pin *pin) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_setPin(pin);
}
inline int32_t gpioPin_clearPin(struct gpio_pin *pin) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_clearPin(pin);
}
inline int32_t gpioPin_togglePin(struct gpio_pin *pin) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_togglePin(pin);
}
inline bool gpioPin_getValue(struct gpio_pin *pin) {
	struct gpio_pin_generic *p = (struct gpio_pin_generic *) pin;
	struct gpio_generic *g = (struct gpio_generic *) p->gpio;
	return g->ops->gpioPin_getValue(pin);
}
#endif
/**\}*/
#endif
