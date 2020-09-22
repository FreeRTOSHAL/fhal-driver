/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef RTC_PRV_
#define RTC_PRV_
#ifndef RTC_PRV
# error "Never include this file out of a RTC driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define RTC_ALREDY_INITED 1
int32_t rtc_genericInit(struct rtc *rtc);
#ifdef CONFIG_RTC_THREAD_SAVE
# define rtc_lock(u, w, e) HAL_LOCK(u, w, e)
# define rtc_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define rtc_lock(u, w, e)
# define rtc_unlock(u, e)
#endif
#define RTC_ADDDEV(ns, p) HAL_ADDDEV(rtc, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(rtc);
#define RTC_GET_DEV(index) HAL_GET_DEV(rtc, index)
#ifndef CONFIG_RTC_MULTI
# define RTC_OPS(ns)
# define RTC_INIT_DEV(ns) 

# define RTC_INIT(ns, index) struct rtc *rtc_init(uint32_t index)
# define RTC_DEINIT(ns, p) int32_t rtc_deinit(struct rtc *p)
# define RTC_ADJUST(ns, p, offset, waitime) int32_t rtc_adjust(struct rtc *p, struct timespec *offset, TickType_t waittime)
# define RTC_GET_TIME(ns, p, time, waittime) int32_t rtc_getTime(struct rtc *p, struct timespec *time, TickType_t waittime)
# define RTC_SET_TIME(ns, p, time, waittime) int32_t rtc_setTime(struct rtc *p, struct timespec *time, TickType_t waittime)
# define RTC_ADJUST_ISR(ns, p, offset) int32_t rtc_adjustISR(struct rtc *p, struct timespec *offset)
# define RTC_GET_TIME_ISR(ns, p, time) int32_t rtc_getTimeISR(struct rtc *p, struct timespec *time)
# define RTC_SET_TIME_ISR(ns, p, time) int32_t rtc_setTimeISR(struct rtc *p, struct timespec *time)
#else
# define RTC_OPS(ns) const struct rtc_ops ns##_rtc_ops = { \
	.rtc_init = &ns##_rtc_init, \
	.rtc_deinit = &ns##_rtc_deinit, \
	.rtc_adjust = &ns##_rtc_adjust, \
	.rtc_getTime = &ns##_rtc_getTime, \
	.rtc_setTime = &ns##_rtc_setTime, \
	.rtc_adjustISR = &ns##_rtc_adjustISR, \
	.rtc_getTimeISR = &ns##_rtc_getTimeISR, \
	.rtc_setTimeISR = &ns##_rtc_setTimeISR, \
}
# define RTC_INIT_DEV(ns) .gen.ops = &ns##_rtc_ops,
# define RTC_INIT(ns, index) static struct rtc *ns##_rtc_init(uint32_t index)
# define RTC_DEINIT(ns, p) static int32_t ns##_rtc_deinit(struct rtc *p)
# define RTC_ADJUST(ns, p, offset, waitime) static int32_t ns##_rtc_adjust(struct rtc *p, struct timespec *offset, TickType_t waittime)
# define RTC_GET_TIME(ns, p, time, waittime) static int32_t ns##_rtc_getTime(struct rtc *p, struct timespec *time, TickType_t waittime)
# define RTC_SET_TIME(ns, p, time, waittime) static int32_t ns##_rtc_setTime(struct rtc *p, struct timespec *time, TickType_t waittime)
# define RTC_ADJUST_ISR(ns, p, offset) static int32_t ns##_rtc_adjustISR(struct rtc *p, struct timespec *offset)
# define RTC_GET_TIME_ISR(ns, p, time) static int32_t ns##_rtc_getTimeISR(struct rtc *p, struct timespec *time)
# define RTC_SET_TIME_ISR(ns, p, time) static int32_t ns##_rtc_setTimeISR(struct rtc *p, struct timespec *time)
#endif
#endif

