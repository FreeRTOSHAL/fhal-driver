#include <counter.h>
#define COUNTER_PRV
#include <counter_prv.h>
#include <counter_software.h>
#include <gpio.h>

static bool counter_gpioIRQ(struct gpio_pin *pin, uint32_t pinID, void *data) {
	struct counter_software *counter = data;
	counter->counter++;
	return false;
}

COUNTER_INIT(software, index, inter) {
	int32_t ret;
	struct counter_software *counter = COUNTER_GET_DEV(index);
	if  (counter == NULL) {
		goto software_counter_init_error0;
	}
	ret = counter_genericInit((struct counter *) counter);
	if (ret < 0) {
		goto software_counter_init_error0;
	}
	if (ret == COUNTER_ALREDY_INITED) {
		goto software_counter_init_exit;
	}

	switch (inter) {
		case COUNTER_RISING:
			counter->inter = GPIO_RISING;
			break;
		case COUNTER_FALLING:
			counter->inter = GPIO_FALLING;
			break;
		case COUNTER_EITHER:
			counter->inter = GPIO_EITHER;
			break;
	}
software_counter_init_exit:
	return (struct counter *) counter;
software_counter_init_error0:
	return NULL;
}
int32_t counter_software_connect(struct counter *c, struct gpio_pin *pin) {
	struct counter_software *counter = (struct counter_software *) c;
	int32_t ret;
	counter->pin = pin;
	counter->counter = 0;
	ret = gpioPin_setCallback(pin, counter_gpioIRQ, c, counter->inter);
	if (ret < 0) {
		return -1;
	}
	ret = gpioPin_enableInterrupt(pin);
	if (ret < 0) {
		return -1;
	}
	return 0;
}
COUNTER_DEINIT(software, c) {
	struct counter_software *counter = (struct counter_software *) c;
	counter->gen.init = false;
	return 0;
}
COUNTER_GET_VALUE(software, c) {
	struct counter_software *counter = (struct counter_software *) c;
	int64_t value = (int64_t) counter->counter;
	return value;
}
COUNTER_RESET(software, c) {
	struct counter_software *counter = (struct counter_software *) c;
	counter->counter = 0;
	return 0;
}
COUNTER_OPS(software);
