#ifndef TLC5947
#define TLC5947
#include <stdint.h>
#include <spi.h>
#include <pwm.h>
#include <gpio.h>

/**
 * \defgroup TLC5947 TLC5947 Driver
 * \ingroup HAL
 * \code
 * #include <tlc5947.h>
 * \endcode
 * 
 * This is the TLC5947 Driver.
 * \{
 */

/**\cond INTERNAL*/

#define PWM_PRV
#include <pwm_prv.h>
#define GPIO_PRV
#include <gpio_prv.h>

struct tlc5947;

struct tlc5947_gpio {
	struct gpio_generic gen;
	struct tlc5947 *tlc;
};

struct tlc5947_gpioPin {
	struct gpio_pin_generic gen;
	struct tlc5947_pwm *pwm;
};

struct tlc5947_pwm {
	struct pwm_generic gen;
	int16_t val;
	struct tlc5947_gpioPin pin;
	struct tlc5947 *tlc;
	int32_t us;
};

struct tlc5947 {
	struct hal gen;
	struct spi_slave *slave;
	struct gpio_pin *black;
	struct tlc5947_pwm *pins[24];
	struct tlc5947_gpio *gpio;
};
extern const struct pwm_ops tlc5947_pwm_ops;
extern const struct gpio_ops tlc5947_gpio_ops;
#define TLC5947_PWM_ADDDEV(id, i) \
	struct tlc5947_pwm tlc5947_##id##_pwm_##i = { \
		.val = 0,\
		.pin = { \
			.gen.gpio = (struct gpio_generic *) &tlc5947_##id##_gpio, \
			.pwm = &tlc5947_##id##_pwm_##i, \
		}, \
		.tlc = &tlc5947_##id, \
		.us = 1000, \
	}; \
	PWM_ADDDEV(tlc5947, tlc5947_##id##_pwm_##i)
/**\endcond*/

int32_t tlc5947_init(int32_t index, struct spi *spi,  uint32_t cs, uint32_t spi_gpio, struct gpio *gpio, uint32_t black);
int32_t tlc5947_deinit(int32_t index);

#define TLC5947_ADDDEV(id) \
	extern struct tlc5947 tlc5947_##id; \
	struct tlc5947_gpio tlc5947_##id##_gpio = { \
		GPIO_INIT_DEV(tlc5947) \
		HAL_NAME("TLC5947 " #id " GPIO") \
		.tlc = &tlc5947_##id, \
	}; \
	TLC5947_PWM_ADDDEV(id, 0); \
	TLC5947_PWM_ADDDEV(id, 1); \
	TLC5947_PWM_ADDDEV(id, 2); \
	TLC5947_PWM_ADDDEV(id, 3); \
	TLC5947_PWM_ADDDEV(id, 4); \
	TLC5947_PWM_ADDDEV(id, 5); \
	TLC5947_PWM_ADDDEV(id, 6); \
	TLC5947_PWM_ADDDEV(id, 7); \
	TLC5947_PWM_ADDDEV(id, 8); \
	TLC5947_PWM_ADDDEV(id, 9); \
	TLC5947_PWM_ADDDEV(id, 10); \
	TLC5947_PWM_ADDDEV(id, 11); \
	TLC5947_PWM_ADDDEV(id, 12); \
	TLC5947_PWM_ADDDEV(id, 13); \
	TLC5947_PWM_ADDDEV(id, 14); \
	TLC5947_PWM_ADDDEV(id, 15); \
	TLC5947_PWM_ADDDEV(id, 16); \
	TLC5947_PWM_ADDDEV(id, 17); \
	TLC5947_PWM_ADDDEV(id, 18); \
	TLC5947_PWM_ADDDEV(id, 19); \
	TLC5947_PWM_ADDDEV(id, 20); \
	TLC5947_PWM_ADDDEV(id, 21); \
	TLC5947_PWM_ADDDEV(id, 22); \
	TLC5947_PWM_ADDDEV(id, 23); \
	GPIO_ADDDEV(tlc5947, tlc5947_##id##_gpio); \
	struct tlc5947 tlc5947_##id = { \
		HAL_NAME("TLC5947 " #id) \
		.slave = NULL, \
		.black = NULL, \
		.gpio = &tlc5947_##id##_gpio,\
		.pins = { \
			&tlc5947_##id##_pwm_0, \
			&tlc5947_##id##_pwm_1, \
			&tlc5947_##id##_pwm_2, \
			&tlc5947_##id##_pwm_3, \
			&tlc5947_##id##_pwm_4, \
			&tlc5947_##id##_pwm_5, \
			&tlc5947_##id##_pwm_6, \
			&tlc5947_##id##_pwm_7, \
			&tlc5947_##id##_pwm_8, \
			&tlc5947_##id##_pwm_9, \
			&tlc5947_##id##_pwm_10, \
			&tlc5947_##id##_pwm_11, \
			&tlc5947_##id##_pwm_12, \
			&tlc5947_##id##_pwm_13, \
			&tlc5947_##id##_pwm_14, \
			&tlc5947_##id##_pwm_15, \
			&tlc5947_##id##_pwm_16, \
			&tlc5947_##id##_pwm_17, \
			&tlc5947_##id##_pwm_18, \
			&tlc5947_##id##_pwm_19, \
			&tlc5947_##id##_pwm_21, \
			&tlc5947_##id##_pwm_22, \
			&tlc5947_##id##_pwm_23, \
		},\
	}; \
	HAL_ADDDEV(hal, tlc5947, tlc5947_##id)

#define TLC5947_ID(id) HAL_GET_ID(hal, tlc5947, tlc5947_##id)
#define TLC5947_GPIO_ID(id) HAL_GET_ID(gpio, tlc5947, tlc5947_##id##_gpio)
#define TLC5947_PWM_ID(id) HAL_GET_ID(pwm, tlc5947, tlc5947_##id##_gpio)
/**\}*/
#endif
