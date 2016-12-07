#include <counter.h>
#define COUNTER_PRV
#include <counter_prv.h>
#include <counter_software.h>
#include <gpio.h>
#ifdef CONFIG_COUNTER_SOFTWARE_DEBUG
#define PRINTF(fmt, ...) printf("SOFT COUNTER: " fmt, ##__VA_ARGS__)
#else
#define PRINTF(fmt, ...)
#endif

static bool counter_gpioIRQ(struct gpio_pin *pin, uint32_t pinID, void *data) {
	struct counter_software *counter = data;
	counter->counter++;
	PRINTF("COUNTER IRQ for: %lu\n", pinID);
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
#ifdef CONFIG_COUNTER_SOFTWARE_DEBUG
#include <FreeRTOS.h>
#include <task.h>
void counter_software_task(void *data) {
	struct counter_software *counter = data;
	bool oldvalue = false;
	bool value;
	vTaskDelay(100 / portTICK_PERIOD_MS);
	for (;;) {
		value = gpioPin_getValue(counter->pin);
		if (value != oldvalue) {
			PRINTF("Value of pin: %p changed\n", counter->pin);
		}
		oldvalue = value;
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
#endif
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
#ifdef CONFIG_COUNTER_SOFTWARE_DEBUG
	xTaskCreate(counter_software_task, "Counter Software Task", 512, c, 1, NULL);
#endif
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
