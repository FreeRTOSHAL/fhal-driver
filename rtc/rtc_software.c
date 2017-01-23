#include <FreeRTOS.h>
#include <stdint.h>
#include <rtc.h>
#include <rtc_software.h>


static bool rtc_software_timerCallback(struct timer *timer, void *data) {
	struct rtc_software *rtc = data;
	rtc->sec++;
	return false;
}

RTC_INIT(software, index) {
	int32_t ret;
	struct rtc_software *rtc = RTC_GET_DEV(index);
	if (rtc == NULL) {
		goto rtc_software_init_error0;
	}
	ret = rtc_genericInit((struct rtc *) rtc);
	if (ret < 0) {
		goto rtc_software_init_error0;
	}
	if (ret == RTC_ALREDY_INITED) {
		goto rtc_software_init_exit;
	}
rtc_software_init_exit:
	return (struct rtc *) rtc;
rtc_software_init_error0:
	return NULL;
}

RTC_DEINIT(software, r) {
	return 0;
}

int32_t rtc_software_connect(struct rtc *r, struct timer *timer) {
	int32_t ret;
	struct rtc_software *rtc = (struct rtc_software *) r;
	rtc->timer = timer;
	rtc->sec = 0;
	ret = timer_setOverflowCallback(rtc->timer, rtc_software_timerCallback, rtc);
	if (ret < 0) {
		goto rtc_software_connect_error0;
	}
	/* Start Timer */
	ret = timer_periodic(rtc->timer, 1000000UL);
	if (ret < 0) {
		goto rtc_software_connect_error1;
	}
	return 0;
rtc_software_connect_error1:
	timer_setOverflowCallback(rtc->timer, NULL, NULL);
rtc_software_connect_error0:
	return -1;
}

RTC_ADJUST(software, r, offset, waitime) {
	/* TODO Support  */
	return -1;
}

RTC_GET_TIME(software, r, time, waitime) {
	int32_t ret;
	rtc_lock(r, waitime, -1);
	ret = rtc_getTimeISR(r, time);
	if (ret < 0) {
		rtc_unlock(r, -1);
		return -1;
	}
	rtc_unlock(r, -1);
	return 0;
}

RTC_SET_TIME(software, r, time, waitime) {
	int ret;
	rtc_lock(r, waitime, -1);
	ret = rtc_setTimeISR(r, time);
	if (ret < 0) {
		rtc_unlock(r, -1);
		return -1;
	}
	rtc_unlock(r, -1);
	return 0;
}

RTC_ADJUST_ISR(software, r, offset) {
	return -1;
}

RTC_GET_TIME_ISR(software, r, time) {
	uint64_t ret;
	struct rtc_software *rtc = (struct rtc_software *) r;
	time->tv_sec = rtc->sec;
	ret = timer_getTime(rtc->timer);
	time->tv_nsec = ret * 1000;
	return 0;
}

RTC_SET_TIME_ISR(software, r, time) {
	int32_t ret;
	struct rtc_software *rtc = (struct rtc_software *) r;
	ret = timer_stop(rtc->timer);
	if (ret < 0) {
		return -1;
	}
	rtc->sec = time->tv_sec;
	/* TODO: Set nsec not supported */
	ret = timer_periodic(rtc->timer, 1000000UL);
	if (ret < 0) {
		return -1;
	}
	return 0;
}
RTC_OPS(software);
