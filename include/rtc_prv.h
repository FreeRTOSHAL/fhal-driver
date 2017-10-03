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
/**
 * \defgroup rtc_driver_prv Driver View form RTC driver 
 * \ingroup rtc_driver
 * 
 * This is the Driver View form RTC Driver. 
 * 
 * This is a implementation of a driver 
 * \include driver/rtc/rtc_dev0.c
 * 
 * Driver with board specific like the MPU9247 has special macros to create new Instances. (for rtc: MPU9250_ADDDEV())
 * \{
 */
/**
 * Error Code Returned if Already inited
 */
#define RTC_ALREDY_INITED 1
/**
 * Generic Init Function
 * 
 * Check Driver Already inited and init mutex if active
 * \param rtc Instants of Driver
 * \return RTC_ALREDY_INITED on alredy init 0 on not init < 0 error
 */
int32_t rtc_genericInit(struct rtc *rtc);
#ifdef CONFIG_RTC_THREAD_SAVE
/**
 * Lock Driver
 * if THREAD_SAVE is not defined this instruction has no function
 */
# define rtc_lock(u, w, e) HAL_LOCK(u, w, e)
/**
 * Unlock Driver
 * if THREAD_SAVE is not defined this instruction has no function
 */
# define rtc_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define rtc_lock(u, w, e)
# define rtc_unlock(u, e)
#endif
/**
 * Add Driver Instance
 * \param ns Driver namespace
 * \param p Pointer to driver instance
 */
#define RTC_ADDDEV(ns, p) HAL_ADDDEV(rtc, ns, p)
/*
 * Define Global Array for Driver Access
 */
HAL_DEFINE_GLOBAL_ARRAY(rtc);
/**
 * Simple function for access a dev from driver
 * \param index index
 * \return see HAL_GET_DEV
 */
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
/**\}*/
#endif

