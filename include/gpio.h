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

/**
 * Init GPIO Subssystem
 * \return GPIO Handle
 */
struct gpio *gpio_init();
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
int32_t gpioPin_setCallback(struct gpio_pin *pin, bool (*callback)(struct gpio_pin *pin, void *data), void *data, enum gpio_interrupt inter);
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
/**\}*/
#endif
