#ifndef COUNTER_SOFTWARE_H_
#define COUNTER_SOFTWARE_H_
/**\cond INTERNAL*/
#include <counter.h>
#define COUNTER_PRV
#include <counter_prv.h>
#include <gpio.h>
extern const struct counter_ops software_counter_ops;
struct counter_software {
	struct counter_generic gen;
	struct gpio_pin *pin;
	uint32_t counter;
	enum gpio_interrupt inter;
};
/**\endcond*/
int32_t counter_software_connect(struct counter *c, struct gpio_pin *pin);
#define ADD_COUNTER_SOFTWARE(id) \
	struct counter_software counter_software_##id = { \
		COUNTER_INIT_DEV(software) \
		HAL_NAME("Software COUNTER " #id) \
	};\
	COUNTER_ADDDEV(software, counter_software_##id)
#define COUNTER_SOFTWARE_ID(id) HAL_GET_ID(counter, software, counter_software_##id)
#endif
