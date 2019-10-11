#include <pwm.h>
#include <tlc5947.h>
#include <pwm_prv.h>
#include <gpio.h>
#include <gpio_prv.h>

static int32_t tlc5947_update(struct tlc5947 *tlc) {
	int32_t ret;
	int i;
	uint16_t send[24];
	uint16_t recv[24];
	for (i = 0; i < 24; i++) {
		send[i] = tlc->pins[i]->val;
		recv[i] = 0;
	}
	ret = spiSlave_transverISR(tlc->slave, send, recv, 24);
	if (ret < 0) {
		return -1;
	}
	ret = spiSlave_transverISR(tlc->slave, send, recv, 24);
	if (ret < 0) {
		return -1;
	}
	for (i = 0; i < 24; i++) {
		if (send[i] != recv[i]) {
			return -1;
		}
	}
	return 0;
}

int32_t tlc5947_init(int32_t index, struct spi *spi,  uint32_t cs, uint32_t spi_gpio, struct gpio *gpio, uint32_t black) {
	struct spi_opt options = {
		.lsb = false,
		.cpol = false,
		.cpha = true,
		.cs = cs,
		.csLowInactive = false,
		.gpio = spi_gpio,
		.size = 12,
		.wdelay = 1,
		.cs_hold = 1,
		.cs_delay = 1,
		.bautrate = 1000000,
	};
	struct tlc5947 *tlc = (struct tlc5947 *) HAL_GET_DEV(hal, index);
	int i;
	int32_t ret;
	if (tlc->gen.init) {
		return 0;
	}
	tlc->slave = spiSlave_init(spi, &options);
	if (!tlc->slave) {
		return -1;
	}
	for (i = 0; i < 24; i++) {
		tlc->pins[i]->val = 0;
	}
	tlc->black = gpioPin_init(gpio, black, GPIO_OUTPUT, GPIO_PULL_UP);
	if (!tlc->black) {
		return -1;
	}
	ret = gpioPin_clearPin(tlc->black);
	if (ret < 0) {
		return -1;
	}
	ret = tlc5947_update(tlc);
	if (ret < 0) {
		gpioPin_deinit(tlc->black);
		return -1;
	}
	tlc->gen.init = true;
	return 0;
}
int32_t tlc5947_deinit(int32_t index) {
	struct tlc5947 *tlc = (struct tlc5947 *) HAL_GET_DEV(hal, index);
	if (!tlc->gen.init) {
		return -1;
	}
	tlc->gen.init = false;
	return spiSlave_deinit(tlc->slave);
}

PWM_INIT(tlc5947, index) {
	int32_t ret;
	struct tlc5947_pwm *pwm;
	pwm = PWM_GET_DEV(index);
	if (pwm == NULL) {
		return NULL;
	}
	ret = pwm_generic_init((struct pwm *) pwm);
	if (ret < 0) {
		return NULL;
	}
	if (ret > 0) {
		return 0;
	}
	if (!pwm->tlc->gen.init) {
		return NULL;
	}
	pwm->gen.init = true;
	pwm->val = 0;
	return pwm;
}
PWM_DEINIT(tlc5947, p) {
	struct tlc5947_pwm *pwm = (struct tlc5947_pwm *) p;
	pwm->gen.init = false;
	return 0;
}
PWM_SET_PERIOD(tlc5947, p, us) {
	struct tlc5947_pwm *pwm = (struct tlc5947_pwm *) p;
	pwm->us = us;
	return 0;
}
PWM_SET_DUTY_CYCLE(tlc5947, p, us) {
	struct tlc5947_pwm *pwm = (struct tlc5947_pwm *) pwm;
	pwm->val = ((us * ((1 << 12) - 1)) / pwm->us);
	return 0;
}

GPIO_INIT(tlc5947, index) {
	int32_t ret;
	struct tlc5947_gpio *gpio;
	gpio = GPIO_GET_DEV(index);
	if (gpio == NULL) {
		return NULL;
	}
	ret = gpio_genericInit((struct gpio *) gpio);
	if (ret < 0) {
		return NULL;
	}
	if (ret > 0) {
		return (struct gpio *) gpio;
	}
	if (!gpio->tlc->gen.init) {
		return NULL;
	}
	gpio->gen.init = true;
	return (struct gpio *) gpio;
}
GPIO_DEINIT(tlc5947, g) {
	struct tlc5947_gpio *gpio = (struct tlc5947_gpio *) g;
	gpio->gen.init = false;
	return 0;
}
GPIO_PIN_INIT(tlc5947, g, p, dir, setting) {
	struct tlc5947_gpio *gpio = (struct tlc5947_gpio *) g;
	struct tlc5947 *tlc;
	struct tlc5947_pwm *pwm;
	struct tlc5947_gpioPin *pin;
	if (gpio != NULL) {
		return NULL;
	}
	tlc = gpio->tlc;
	if (p > 24) {
		return NULL;
	}
	if (!tlc->gen.init) {
		return NULL;
	}
	pwm = tlc->pins[p];
	pin = &pwm->pin;
	return (struct gpio_pin *) pin;
}
GPIO_PIN_DEINIT(tlc5947, p) {
	return 0;
}
GPIO_PIN_ENABLE_INTERRUPT(tlc5947, p) {
	return -1;
}
GPIO_PIN_DISABLE_INTERRUPT(tlc5947, p) {
	return -1;
}
GPIO_PIN_SET_CALLBACK(tlc5947, p, callback, data, inter) {
	return -1;
}
GPIO_PIN_SET_DIRECTION(tlc5947, p, dir) {
	if (dir != GPIO_OUTPUT) {
		return -1;
	}
	return 0;
}
GPIO_PIN_SET_SETTING(tlc5947, p, setting) {
	/* is ok */
	return 0;
}
GPIO_PIN_SCHMITT_TRIGGER(tlc5947, p, schmitt) {
	return -1;
}
GPIO_PIN_SET_VALUE(tlc5947, p, value) {
	if (value) {
		return gpioPin_setPin(p);
	} else {
		return gpioPin_clearPin(p);
	}
}
GPIO_PIN_SET_PIN(tlc5947, p) {
	struct tlc5947_gpioPin *pin = (struct tlc5947_gpioPin *) p;
	pin->pwm->val = 1000;
	return tlc5947_update(pin->pwm->tlc);
}
GPIO_PIN_CLEAR_PIN(tlc5947, p) {
	struct tlc5947_gpioPin *pin = (struct tlc5947_gpioPin *) p;
	pin->pwm->val = 0;
	return tlc5947_update(pin->pwm->tlc);
}
GPIO_PIN_TOGGLE_PIN(tlc5947, p) {
	struct tlc5947_gpioPin *pin = (struct tlc5947_gpioPin *) p;
	pin->pwm->val = (pin->pwm->val == 0) ? 1000 : 0;
	return tlc5947_update(pin->pwm->tlc);
}
GPIO_PIN_GET_VALUE(tlc5947, p) {
	return -1;
}
GPIO_OPS(tlc5947);
PWM_OPS(tlc5947);
