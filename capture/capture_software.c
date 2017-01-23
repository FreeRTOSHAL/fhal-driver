#include <capture.h>
#include <capture_software.h>
#include <stdint.h>
#include <stdbool.h>

static bool capture_software_callback(struct gpio_pin *pin, uint32_t pinID, void *data) {
	struct capture_software *capture = data;
	bool higherTaskHasWoken = false;
	struct timespec time; 
	int32_t ret;
	ret = rtc_getTimeISR(capture->rtc, &time);
	CONFIG_ASSERT(ret < 0);
	capture->time = (((uint64_t) time.tv_sec) - ((uint64_t) capture->oldtime.tv_sec)) * 1000000ULL;
	if (time.tv_nsec > capture->oldtime.tv_nsec) {
		capture->time += (time.tv_nsec - capture->oldtime.tv_nsec) / 1000ULL;
	} else {
		capture->time += ((1000000000ULL - capture->oldtime.tv_nsec) + time.tv_nsec) / 1000ULL;
	}
	capture->oldtime = time;
	if (capture->callback) {
		higherTaskHasWoken = capture->callback((struct capture *) capture, 0, capture->time, capture->data);
	}
	return higherTaskHasWoken;
}

CAPTURE_INIT(software, index) {
	int32_t ret;
	struct capture_software *capture = CAPTURE_GET_DEV(index);
	if (capture == NULL) {
		goto capture_software_init_ereror0;
	}
	ret = capture_generic_init((struct capture *) capture);
	if (ret < 0) {
		goto capture_software_init_ereror0;
	}
	if (ret == CAPTURE_ALREDY_INITED) {
		goto capture_software_init_exit;
	}
capture_software_init_exit:
	return 0;
capture_software_init_ereror0:
	return NULL;
}

CAPTURE_DEINIT(software, c) {
	return 0;
}
int32_t capture_software_connect(struct capture *c, struct gpio_pin *pin, struct rtc *rtc) {
	int32_t ret;
	struct capture_software *capture = (struct capture_software *) c;
	capture->pin = pin;
	capture->rtc = rtc;
	capture->time = 0;
	capture->callback = NULL;
	capture->data = NULL;
	ret = gpioPin_setCallback(pin, capture_software_callback, capture, GPIO_EITHER);
	if (ret < 0) {
		goto capture_software_connect_error0;
	}
	/* Set Old Time to current time */
	ret = rtc_getTimeISR(rtc, &capture->oldtime);
	if (ret < 0) {
		goto capture_software_connect_error1;
	}
	ret = gpioPin_enableInterrupt(pin);
	if (ret < 0) {
		goto capture_software_connect_error1;
	}
	return 0;
capture_software_connect_error1:
	gpioPin_setCallback(pin, NULL, NULL, GPIO_FALLING);
capture_software_connect_error0:
	return -1;
}

CAPTURE_SET_CALLBACK(software, c, callback, data) {
	struct capture_software *capture = (struct capture_software *) c;
	capture->data = data;
	capture->callback = callback;
	return 0;
}

CAPTURE_SET_PERIOD(software, c, us) {
	/* Not Supported */
	return 0;
}

CAPTURE_GET_TIME(software, c) {
	/* Not Supported */
	return -1;
}

CAPTURE_GET_CHANNEL_TIME(software, c)  {
	struct capture_software *capture = (struct capture_software *) c;
	return capture->time;
}

CAPTURE_OPS(software);
