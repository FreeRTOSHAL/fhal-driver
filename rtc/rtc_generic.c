/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <rtc.h>
#define RTC_PRV
#include <rtc_prv.h>

int32_t rtc_genericInit(struct rtc *t) {
	struct rtc_generic *rtc = (struct rtc_generic *) t;
	if (hal_isInit(rtc)) {
		return RTC_ALREDY_INITED;
	}
#ifdef CONFIG_RTC_THREAD_SAFE
	{
		int32_t ret = hal_init(rtc);
		if (ret < 0) {
			goto rtc_generic_init_error0;
		}
	}
#endif
	rtc->init = true;
	return 0;
#ifdef CONFIG_RTC_THREAD_SAFE
rtc_generic_init_error0:
	return -1;
#endif

}
#ifdef CONFIG_RTC_MULTI
struct rtc *rtc_init(uint32_t index);
int32_t rtc_deinit(struct rtc *rtc);
int32_t rtc_adjust(struct rtc *rtc, struct timespec *offset, TickType_t waittime);
int32_t rtc_getTime(struct rtc *rtc, struct timespec *time, TickType_t waittime);
int32_t rtc_setTime(struct rtc *rtc, struct timespec *time, TickType_t waittime);
int32_t rtc_adjustISR(struct rtc *rtc, struct timespec *offset);
int32_t rtc_getTimeISR(struct rtc *rtc, struct timespec *time);
int32_t rtc_setTimeISR(struct rtc *rtc, struct timespec *time);
#endif

