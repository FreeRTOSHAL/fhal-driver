/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#include <rtc.h>
#define RTC_PRV
#include <rtc_prv.h>

int32_t rtc_genericInit(struct rtc *t) {
	struct rtc_generic *rtc = (struct rtc_generic *) t;
	if (hal_isInit(rtc)) {
		return RTC_ALREDY_INITED;
	}
#ifdef CONFIG_RTC_THREAD_SAVE
	{
		int32_t ret = hal_init(rtc);
		if (ret < 0) {
			goto rtc_generic_init_error0;
		}
	}
#endif
	rtc->init = true;
	return 0;
#ifdef CONFIG_RTC_THREAD_SAVE
rtc_generic_init_error0:
	return -1;
#endif

}
#ifdef CONFIG_RTC_MULTI
struct rtc *rtc_init(uint32_t index);
int32_t rtc_deinit(struct rtc *rtc);
int32_t rtc_adjust(struct rtc *rtc, struct timespec *offset, Ticktype_t waittime);
int32_t rtc_getTime(struct rtc *rtc, struct timespec *time, Ticktype_t waittime);
int32_t rtc_setTime(struct rtc *rtc, struct timespec *time, Ticktype_t waittime);
int32_t rtc_adjustISR(struct rtc *rtc, struct timespec *offset);
int32_t rtc_getTimeISR(struct rtc *rtc, struct timespec *time, Ticktype_t waittime);
int32_t rtc_setTimeISR(struct rtc *rtc, struct timespec *time, Ticktype_t waittime);
#endif

