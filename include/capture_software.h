#ifndef CAPTURE_SOFTWARE_H_
#define CAPTURE_SOFTWARE_H_
#include <gpio.h>
#include <rtc.h>
/**\cond INTERNAL*/
#include <capture.h>
#define CAPTURE_PRV
#include <capture_prv.h>
struct capture_software {
	struct capture_generic gen;
	struct gpio_pin *pin;
	struct rtc *rtc;
	bool (*callback)(struct capture *capture, uint32_t index, uint64_t tiem, void *data);
	void *data;
	struct timespec oldtime;
	uint64_t time;
	uint32_t index;
};
extern const struct capture_ops software_capture_ops;
/**\endcond*/
int32_t capture_software_connect(struct capture *c, struct gpio_pin *pin, struct rtc *rtc);

#define ADD_CAPTURE_SOFTWARE(id) \
	struct capture_software capture_software_##id = { \
		CAPTURE_INIT_DEV(software) \
		HAL_NAME("Software Capture " #id) \
	}; \
	CAPTURE_ADDDEV(software, capture_software_##id)
#define CAPTURE_SOFTWARE_ID(id) HAL_GET_ID(capture, software, capture_software_##id)
#endif
